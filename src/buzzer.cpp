#include "buzzer.hpp"

extern "C" {
#include <cstdlib>

#include "iodefine.h"
}

Buzzer* Buzzer::buzzer_p = nullptr;

Buzzer* Buzzer::getInstance() {
    if (!buzzer_p) {
        buzzer_p = (Buzzer*)malloc(sizeof(Buzzer));
    }

    return buzzer_p;
}

void Buzzer::init() {
    // MTU2有効化
    SYSTEM.PRCR.WORD = 0xA502;
    MSTP(MTU2) = 0;
    SYSTEM.PRCR.WORD = 0xA500;

    // プロテクト解除
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;
    // MTIOC2A
    MPC.PB5PFS.BIT.PSEL = 1;
    // 再プロテクト
    MPC.PWPR.BYTE = 0x80;

    // PB5を周辺機能で使用
    PORTB.PMR.BIT.B5 = 1;

    // カウント停止
    MTU.TSTR.BIT.CST2 = 0;

    // カウンタクロックの選択
    // PCLK/4分周，48MHz
    MTU2.TCR.BIT.TPSC = 1;
    // 立ち上がりでカウント
    MTU2.TCR.BIT.CKEG = 0;

    // カウンタクリア要因の選択
    // TCNT==TGRAでカウンタクリア
    MTU2.TCR.BIT.CCLR = 1;

    // PWMモード1
    MTU2.TMDR.BIT.MD = 2;

    // アウトプットコンペアレジスタ
    // 初期出力Low，コンペマッチでLow
    MTU2.TIOR.BIT.IOA = 1;
    // 初期出力Low，コンペアマッチでHigh
    MTU2.TIOR.BIT.IOB = 2;

    // 周期設定
    MTU2.TGRA = 0;
    MTU2.TGRB = 0;

    MTU2.TCNT = 0;

    // カウント動作開始
    MTU.TSTR.BIT.CST2 = 1;
}

void Buzzer::setFreq(float freq) {
    frequency = freq;
    if (MTU2.TGRA) {
        on();
    }
}

void Buzzer::on() {
    MTU2.TGRA = 12e6 / frequency - 1;
    MTU2.TGRB = 6e6 / frequency - 1;
}

void Buzzer::off() {
    MTU2.TGRA = 0;
    MTU2.TGRB = 0;
}
