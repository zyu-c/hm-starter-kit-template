#include "motor.hpp"

extern "C" {
#include <cstdlib>

#include "iodefine.h"
}

Motor* Motor::motor_p = nullptr;

Motor* Motor::getInstance() {
    if (!motor_p) {
        motor_p = (Motor*)malloc(sizeof(Motor));
    }

    return motor_p;
}

void Motor::init() {
    // MTU0有効化
    SYSTEM.PRCR.WORD = 0xA502;
    MSTP(MTU0) = 0;
    SYSTEM.PRCR.WORD = 0xA500;

    // プロテクト解除
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;
    // PWM出力設定
    // 右，MTIOC0C
    MPC.PB1PFS.BIT.PSEL = 1;
    // 左，MTIOC0A
    MPC.PB3PFS.BIT.PSEL = 1;
    // 再プロテクト
    MPC.PWPR.BYTE = 0x80;

    // B3,B1を周辺機能で使用
    PORTB.PMR.BIT.B3 = 1;
    PORTB.PMR.BIT.B1 = 1;

    // カウント停止
    MTU.TSTR.BIT.CST0 = 0;

    // モータ時定数 t = L/R = 17uH/(1.07+0.5+0.3) = 110kHz

    // 分周器設定，PCLK/1分周，48MHz
    MTU0.TCR.BIT.TPSC = 0;
    // 立ち上がりでカウント
    MTU0.TCR.BIT.CKEG = 0;

    // カウンタクリア要因の選択
    // TCNT==TGRDでカウンタクリア
    MTU0.TCR.BIT.CCLR = 6;

    // PWMモード2
    MTU0.TMDR.BIT.MD = 3;

    // アウトプットコンペアレジスタ
    // 初期出力High，コンペアマッチでLow
    MTU0.TIORH.BIT.IOA = 5;
    // 初期出力High，コンペアマッチでLow
    MTU0.TIORL.BIT.IOC = 5;
    // 初期出力Low，コンペアマッチでHigh
    MTU0.TIORL.BIT.IOD = 2;

    // 周期設定
    MTU0.TGRA = 0;
    MTU0.TGRC = 0;
    // 200kHz
    MTU0.TGRD = 240 - 1;

    MTU0.TCNT = 0;

    // カウント動作開始
    MTU.TSTR.BIT.CST0 = 1;

    // ポート出力設定
    PORTC.PDR.BIT.B6 = 1;
    PORTC.PDR.BIT.B5 = 1;
    PORTC.PDR.BIT.B4 = 1;

    //スリープ解除
    PORTC.PODR.BIT.B6 = 1;
}

void Motor::setDuty(uint8_t port, float duty) {
    switch (port) {
        case 0:
            if (duty >= 0) {
                PORTC.PODR.BIT.B4 = 1;
                MTU0.TGRA = duty * 240;
            } else {
                PORTC.PODR.BIT.B4 = 0;
                MTU0.TGRA = duty * -240;
            }
            return;
        case 1:
            if (duty >= 0) {
                PORTC.PODR.BIT.B5 = 0;
                MTU0.TGRC = duty * 240;
            } else {
                PORTC.PODR.BIT.B5 = 1;
                MTU0.TGRC = duty * -240;
            }
            return;
        default:
            return;
    }
}
