#include "encoder.hpp"

extern "C" {
#include <cstdlib>

#include "iodefine.h"
}

Encoder* Encoder::encoder_p = nullptr;

Encoder* Encoder::getInstance() {
    if (!encoder_p) {
        encoder_p = (Encoder*)malloc(sizeof(Encoder));
    }

    return encoder_p;
}

void Encoder::init() {
    // RSPI0有効化
    SYSTEM.PRCR.WORD = 0xA502;
    MSTP(RSPI0) = 0;
    SYSTEM.PRCR.WORD = 0xA500;

    // スレーブセレクト極性レジスタ(SSLP)の設定
    // SSL0はアクティブLow
    RSPI0.SSLP.BIT.SSL0P = 0;
    // SSL2はアクティブLow
    RSPI0.SSLP.BIT.SSL2P = 0;

    // RSPI端子制御レジスタ(SPPCR)の設定
    // 通常モード
    RSPI0.SPPCR.BYTE = 0x00;

    // RSPIビットレートレジスタ(SPBR)の設定
    // 分周比 1/48 1MHz
    RSPI0.SPBR = 1;

    // RSPIデータコントロールレジスタ(SPDCR)の設定
    // フレーム数は1
    RSPI0.SPDCR.BIT.SPFC = 0;
    // SPDRは受信バッファ
    RSPI0.SPDCR.BIT.SPRDTD = 0;
    // SPDRはワードアクセス
    RSPI0.SPDCR.BIT.SPLW = 0;

    // RSPIクロック遅延レジスタ(SSLND)の設定
    RSPI0.SSLND.BIT.SLNDL = 2;

    // RSPI次アクセス遅延レジスタ(SPND)の設定
    RSPI0.SPND.BIT.SPNDL = 2;

    // RSPI制御レジスタ２(SPCR2)の設定
    RSPI0.SPCR2.BIT.SPPE = 1;

    // RSPIシーケンス制御レジスタ(SPSCR)の設定
    RSPI0.SPSCR.BIT.SPSLN = 0;

    // RSPIコマンドレジスタ(SPCMD)の設定
    RSPI0.SPCMD0.BIT.CPHA = 1;
    RSPI0.SPCMD0.BIT.CPOL = 1;
    RSPI0.SPCMD0.BIT.BRDV = 0;
    RSPI0.SPCMD0.BIT.SSLA = 0;
    RSPI0.SPCMD0.BIT.SSLKP = 0;
    RSPI0.SPCMD0.BIT.SPB = 9;
    RSPI0.SPCMD0.BIT.LSBF = 0;
    RSPI0.SPCMD0.BIT.SPNDEN = 1;
    RSPI0.SPCMD0.BIT.SLNDEN = 1;
    RSPI0.SPCMD0.BIT.SCKDEN = 1;

    // 割り込みコントローラの設定
    // 割り込み禁止
    ICU.IER[4].BIT.IEN7 = 0;
    ICU.IER[5].BIT.IEN0 = 0;
    ICU.IER[5].BIT.IEN1 = 0;
    // 割り込み優先度設定
    ICU.IPR[39].BIT.IPR = 15;
    // 割り込み許可
    ICU.IER[4].BIT.IEN7 = 1;
    ICU.IER[5].BIT.IEN0 = 1;
    ICU.IER[5].BIT.IEN1 = 1;

    // 入出力ポートの設定
    // PB0を有効化
    PORT.PSRB.BIT.PSEL0 = 0;
    // PA1を周辺機能として使用
    PORTA.PMR.BIT.B1 = 1;
    // PA4を周辺機能として使用
    PORTA.PMR.BIT.B4 = 1;
    // PA6を周辺機能として使用
    PORTA.PMR.BIT.B6 = 1;
    // PB0を周辺機能として使用
    PORTB.PMR.BIT.B0 = 1;
    // PC7を周辺機能として使用
    PORTC.PMR.BIT.B7 = 1;

    // プロテクト解除
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;
    // 出力設定
    // PA1をSSLA2として利用
    MPC.PA1PFS.BIT.PSEL = 13;
    // PA4をSSLA0として利用
    MPC.PA4PFS.BIT.PSEL = 13;
    // PA6をMOSIAとして利用
    MPC.PA6PFS.BIT.PSEL = 13;
    // PB0をRSPCKAとして利用
    MPC.PB0PFS.BIT.PSEL = 13;
    // PC7をMISOAとして利用
    MPC.PC7PFS.BIT.PSEL = 13;
    // 再プロテクト
    MPC.PWPR.BYTE = 0x80;

    // RSPI制御レジスタ(SCPR)の設定
    // SPI動作(4線式)
    RSPI0.SPCR.BIT.SPMS = 0;
    // 全二重同期式シリアル通信
    RSPI0.SPCR.BIT.TXMD = 0;
    // モードフォルトエラー検出を禁止
    RSPI0.SPCR.BIT.MODFEN = 0;
    // マスターモード
    RSPI0.SPCR.BIT.MSTR = 1;
    // RSPI機能が無効
    RSPI0.SPCR.BIT.SPE = 0;
    // RSPIエラー割り込み要求の発生を禁止
    RSPI0.SPCR.BIT.SPEIE = 0;
    // RSPI送信割り込み要求の発生を禁止
    RSPI0.SPCR.BIT.SPTIE = 0;
    // RSPI受信割り込み要求の発生を禁止
    RSPI0.SPCR.BIT.SPRIE = 0;

    // RSPI制御レジスタ(SPCR)のリード
    RSPI0.SPCR.BYTE;
}

void Encoder::preprocess() {
    finished_write = false;

    // エラー要因のクリア
    // モードフォルトエラーフラグ
    RSPI0.SPSR.BIT.MODF = 0;
    // オーバーランエラーフラグ
    RSPI0.SPSR.BIT.OVRF = 0;
    // パリティエラーフラグ
    RSPI0.SPSR.BIT.PERF = 0;

    // RSPIアイドル割り込みを禁止
    RSPI0.SPCR2.BIT.SPIIE = 0;

    // SPEビットの許可
    // 割り込みの許可
    // RSPI送信割り込み要求の発生を許可
    RSPI0.SPCR.BIT.SPTIE = 1;
    // RSPI受信割り込み要求の発生を許可
    RSPI0.SPCR.BIT.SPRIE = 1;
    // RSPIを有効化
    RSPI0.SPCR.BIT.SPE = 1;

    while (RSPI0.SPCR.BIT.SPTIE) {
    }
    while (RSPI0.SPCR2.BIT.SPIIE) {
    }
    while (RSPI0.SPCR.BIT.SPRIE) {
    }
}

void Encoder::int_write() {
    if (!finished_write) {
        RSPI0.SPDR.WORD.H = 0x0000;
        // 送信割り込み禁止
        RSPI0.SPCR.BIT.SPTIE = 0;
        // アイドル割り込み許可
        RSPI0.SPCR2.BIT.SPIIE = 1;
    }
    finished_write = true;
}

void Encoder::int_idle() {
    RSPI0.SPCR.BIT.SPE = 0;
    RSPI0.SPCR2.BIT.SPIIE = 0;
}

void Encoder::int_read() {
    data_tmp = RSPI0.SPDR.WORD.H;
    RSPI0.SPCR.BIT.SPRIE = 0;
}

void Encoder::update() {
    // 右エンコーダ値取得
    RSPI0.SPCMD0.BIT.SSLA = 0;
    preprocess();
    position[0] = 1023 - data_tmp;

    // 左エンコーダ値取得
    RSPI0.SPCMD0.BIT.SSLA = 2;
    preprocess();
    position[1] = data_tmp;
}

uint16_t Encoder::getPosition(uint8_t port) { return position[port]; }
