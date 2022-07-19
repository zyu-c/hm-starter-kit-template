#include "ioex.hpp"

extern "C" {
#include <cstdlib>

#include "iodefine.h"
}

Ioex* Ioex::ioex_p = nullptr;

Ioex* Ioex::getInstance() {
    if (!ioex_p) {
        ioex_p = (Ioex*)malloc(sizeof(Ioex));
    }

    return ioex_p;
}

void Ioex::init() {
    //入出力ポートの設定
    // P16を周辺機能として使用
    PORT1.PMR.BIT.B6 = 1;
    // P17を周辺機能として使用
    PORT1.PMR.BIT.B7 = 1;

    // プロテクト解除
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;
    // 出力設定
    // P16をSCL2として利用
    MPC.P16PFS.BIT.PSEL = 15;
    // P17をSDA2として利用
    MPC.P17PFS.BIT.PSEL = 15;
    // 再プロテクト
    MPC.PWPR.BYTE = 0x80;

    // RSPI1有効化
    SYSTEM.PRCR.WORD = 0xA502;
    MSTP(RIIC2) = 0;
    SYSTEM.PRCR.WORD = 0xA500;

    // SCLn，SDAn端子被駆動状態
    RIIC2.ICCR1.BIT.ICE = 0;
    // RIICリセット
    RIIC2.ICCR1.BIT.IICRST = 1;
    // 内部リセット
    RIIC2.ICCR1.BIT.ICE = 1;

    // 通信ビットレートを設定
    // ICMR1.CKSを設定
    // ICBRL/ICBRHを設定
    RIIC2.ICMR1.BIT.CKS = 2;
    RIIC2.ICBRH.BIT.BRH = 7;
    RIIC2.ICBRL.BIT.BRL = 16;

    // ICMR2，ICMR3を設定
    RIIC2.ICMR2.BYTE = 0x00;
    RIIC2.ICMR3.BYTE = 0x00;
    RIIC2.ICMR3.BIT.ACKWP = 1;

    // ICFERを設定
    RIIC2.ICFER.BYTE = 0x72;

    // 内部リセット解除
    RIIC2.ICCR1.BIT.IICRST = 0;

    setupSlave();
}

void Ioex::setupSlave() {
    // 入出力設定
    uint8_t data[2] = {0x03, 0xF0};
    write(data, 2);
}

void Ioex::write(uint8_t data[], uint8_t size) {
    // バス占有状態判定
    while (RIIC2.ICCR2.BIT.BBSY) {
    }

    //スタートコンディション発行
    RIIC2.ICCR2.BIT.ST = 1;

    // スレーブアドレス，データ送信
    if (!RIIC2.ICSR2.BIT.NACKF) {
        // アドレス
        while (!RIIC2.ICSR2.BIT.TDRE) {
        }
        RIIC2.ICDRT = 0x30;

        // データ
        for (int i = 0; i < size; i++) {
            while (!RIIC2.ICSR2.BIT.TDRE) {
            }
            RIIC2.ICDRT = data[i];
        }

        // 最終データ送信確認
        while (!RIIC2.ICSR2.BIT.TEND) {
        }
    }

    // ストップコンディション発行
    RIIC2.ICSR2.BIT.STOP = 0;
    RIIC2.ICCR2.BIT.SP = 1;

    //ストップコンディション発行確認
    while (!RIIC2.ICSR2.BIT.STOP) {
    }

    //次通信のための処理
    RIIC2.ICSR2.BIT.NACKF = 0;
    RIIC2.ICSR2.BIT.STOP = 0;
}

uint8_t Ioex::read() {
    uint8_t data[1] = {0x00};
    write(data, 1);

    uint8_t ans;

    // バス占有状態判定
    while (RIIC2.ICCR2.BIT.BBSY) {
    }

    //スタートコンディション発行
    RIIC2.ICCR2.BIT.ST = 1;

    // アドレス
    while (!RIIC2.ICSR2.BIT.TDRE) {
    }
    RIIC2.ICDRT = 0x31;

    // 受信待ち
    while (!RIIC2.ICSR2.BIT.RDRF) {
    }

    // スレーブアドレス，データ送信
    if (!RIIC2.ICSR2.BIT.NACKF) {
        // wait設定
        RIIC2.ICMR3.BIT.WAIT = 1;

        RIIC2.ICDRR;
        while (!RIIC2.ICSR2.BIT.RDRF) {
        }

        RIIC2.ICMR3.BIT.RDRFS = 1;
        RIIC2.ICMR3.BIT.ACKBT = 1;
        ans = (uint8_t)(RIIC2.ICDRR);
        while (!RIIC2.ICSR2.BIT.RDRF) {
        }

        // ストップコンディション発行
        RIIC2.ICSR2.BIT.STOP = 0;
        RIIC2.ICCR2.BIT.SP = 1;
        RIIC2.ICDRR;

        RIIC2.ICMR3.BIT.ACKBT = 1;
        RIIC2.ICMR3.BIT.WAIT = 0;
    } else {
        RIIC2.ICSR2.BIT.STOP = 0;
        RIIC2.ICCR2.BIT.SP = 1;
        RIIC2.ICDRR;
    }

    //ストップコンディション発行確認
    while (!RIIC2.ICSR2.BIT.STOP) {
    }

    //次通信のための処理
    RIIC2.ICMR3.BIT.RDRFS = 0;
    RIIC2.ICMR3.BIT.ACKBT = 0;
    RIIC2.ICSR2.BIT.NACKF = 0;
    RIIC2.ICSR2.BIT.STOP = 0;

    return ans;
}

void Ioex::update() {
    // LED更新
    uint8_t data[2] = {0x01, 0x00};
    data[1] |= (led_state & 0x08) >> 1;
    data[1] |= (led_state & 0x04) << 1;
    data[1] |= (led_state & 0x02) >> 1;
    data[1] |= (led_state & 0x01) << 1;
    data[1] = ~data[1];
    write(data, 2);

    // Button更新
    button_state = read() >> 4;
}

void Ioex::on(uint8_t port) { led_state |= (0x01 << port); }

void Ioex::off(uint8_t port) { led_state &= ~(0x01 << port); }

void Ioex::toggle(uint8_t port) { led_state ^= (0x01 << port); }

void Ioex::set(uint8_t state) { led_state = state; }

bool Ioex::getState(uint8_t port) { return button_state & (0x01 << port); }
