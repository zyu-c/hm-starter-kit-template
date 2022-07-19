#include "gyro.hpp"

extern "C" {
#include <cstdlib>

#include "iodefine.h"
}

#define MY_PI 3.14159265358979323

Gyro* Gyro::gyro_p = nullptr;

Gyro* Gyro::getInstance() {
    if (!gyro_p) {
        gyro_p = (Gyro*)malloc(sizeof(Gyro));
    }

    return gyro_p;
}

void Gyro::init() {
    // RSPI1有効化
    SYSTEM.PRCR.WORD = 0xA502;
    MSTP(RSPI1) = 0;
    SYSTEM.PRCR.WORD = 0xA500;

    // スレーブセレクト極性レジスタ(SSLP)の設定
    // SSL0はアクティブLow
    RSPI1.SSLP.BIT.SSL0P = 0;

    // RSPI端子制御レジスタ(SPPCR)の設定
    // 通常モード
    RSPI1.SPPCR.BYTE = 0x00;

    // RSPIビットレートレジスタ(SPBR)の設定
    // 分周比 1/48 1MHz
    RSPI1.SPBR = 5;

    // RSPIデータコントロールレジスタ(SPDCR)の設定
    // フレーム数は1
    RSPI1.SPDCR.BIT.SPFC = 0;
    // SPDRは受信バッファ
    RSPI1.SPDCR.BIT.SPRDTD = 0;
    // SPDRはロングワードアクセス
    RSPI1.SPDCR.BIT.SPLW = 1;

    // RSPIクロック遅延レジスタ(SSLND)の設定
    RSPI1.SSLND.BIT.SLNDL = 2;

    // RSPI次アクセス遅延レジスタ(SPND)の設定
    RSPI1.SPND.BIT.SPNDL = 2;

    // RSPI制御レジスタ２(SPCR2)の設定
    RSPI1.SPCR2.BIT.SPPE = 0;

    // RSPIシーケンス制御レジスタ(SPSCR)の設定
    RSPI1.SPSCR.BIT.SPSLN = 0;

    // RSPIコマンドレジスタ(SPCMD)の設定
    RSPI1.SPCMD0.BIT.CPHA = 1;
    RSPI1.SPCMD0.BIT.CPOL = 1;
    RSPI1.SPCMD0.BIT.BRDV = 1;
    RSPI1.SPCMD0.BIT.SSLA = 0;
    RSPI1.SPCMD0.BIT.SSLKP = 0;
    RSPI1.SPCMD0.BIT.SPB = 15;
    RSPI1.SPCMD0.BIT.LSBF = 0;
    RSPI1.SPCMD0.BIT.SPNDEN = 0;
    RSPI1.SPCMD0.BIT.SLNDEN = 1;
    RSPI1.SPCMD0.BIT.SCKDEN = 1;

    // 割り込みコントローラの設定
    // 割り込み禁止
    ICU.IER[5].BIT.IEN2 = 0;
    ICU.IER[5].BIT.IEN3 = 0;
    ICU.IER[5].BIT.IEN4 = 0;
    // 割り込み優先度設定
    ICU.IPR[42].BIT.IPR = 15;
    // 割り込み許可
    ICU.IER[5].BIT.IEN2 = 1;
    ICU.IER[5].BIT.IEN3 = 1;
    ICU.IER[5].BIT.IEN4 = 1;

    // 入出力ポートの設定
    // P27を周辺機能として使用
    PORT2.PMR.BIT.B7 = 1;
    // PE2を周辺機能として使用
    PORTE.PMR.BIT.B2 = 1;
    // PE3を周辺機能として使用
    PORTE.PMR.BIT.B3 = 1;
    // PE4を周辺機能として使用
    PORTE.PMR.BIT.B4 = 1;

    // プロテクト解除
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;
    // 出力設定
    // P27をRSPCKBとして利用
    MPC.P27PFS.BIT.PSEL = 13;
    // PE2をMOSIBとして利用
    MPC.PE2PFS.BIT.PSEL = 14;
    // PE3をMISOBとして利用
    MPC.PE3PFS.BIT.PSEL = 13;
    // PE4をSSLB0として利用
    MPC.PE4PFS.BIT.PSEL = 13;
    // 再プロテクト
    MPC.PWPR.BYTE = 0x80;

    // RSPI制御レジスタ(SCPR)の設定
    // SPI動作(4線式)
    RSPI1.SPCR.BIT.SPMS = 0;
    // 全二重同期式シリアル通信
    RSPI1.SPCR.BIT.TXMD = 0;
    // モードフォルトエラー検出を禁止
    RSPI1.SPCR.BIT.MODFEN = 0;
    // マスターモード
    RSPI1.SPCR.BIT.MSTR = 1;
    // RSPI機能が無効
    RSPI1.SPCR.BIT.SPE = 0;
    // RSPIエラー割り込み要求の発生を禁止
    RSPI1.SPCR.BIT.SPEIE = 0;
    // RSPI送信割り込み要求の発生を禁止
    RSPI1.SPCR.BIT.SPTIE = 0;
    // RSPI受信割り込み要求の発生を禁止
    RSPI1.SPCR.BIT.SPRIE = 0;

    // RSPI制御レジスタ(SPCR)のリード
    RSPI1.SPCR.BYTE;

    setupSlave();
}

void Gyro::setupSlave() {
    // Bank2へ
    preprocess_write(0x7F, 0x20);
    // Gyroを2000dps化
    preprocess_write(0x01, 0x07);

    // Bank0へ
    preprocess_write(0x7F, 0x00);
    // スタート
    preprocess_write(0x06, 0x21);
}

void Gyro::preprocess_write(uint8_t address, uint8_t data) {
    finished_write = false;
    send_data = (address << 8) | data;

    // データ長16bit
    RSPI1.SPCMD0.BIT.SPB = 15;

    // エラー要因のクリア
    // モードフォルトエラーフラグ
    RSPI1.SPSR.BIT.MODF = 0;
    // オーバーランエラーフラグ
    RSPI1.SPSR.BIT.OVRF = 0;
    // パリティエラーフラグ
    RSPI1.SPSR.BIT.PERF = 0;

    // RSPIアイドル割り込みを禁止
    RSPI1.SPCR2.BIT.SPIIE = 0;

    // SPEビットの許可
    // 割り込みの許可
    // RSPI送信割り込み要求の発生を許可
    RSPI1.SPCR.BIT.SPTIE = 1;
    // RSPI受信割り込み要求の発生を許可
    RSPI1.SPCR.BIT.SPRIE = 1;
    // RSPIを有効化
    RSPI1.SPCR.BIT.SPE = 1;

    while (RSPI1.SPCR.BIT.SPTIE) {
    }
    while (RSPI1.SPCR2.BIT.SPIIE) {
    }
    while (RSPI1.SPCR.BIT.SPRIE) {
    }
}

void Gyro::preprocess_read(uint8_t address) {
    finished_write = false;
    send_data = (address | 0x80) << 16;

    // データ長24bit
    RSPI1.SPCMD0.BIT.SPB = 1;

    // エラー要因のクリア
    // モードフォルトエラーフラグ
    RSPI1.SPSR.BIT.MODF = 0;
    // オーバーランエラーフラグ
    RSPI1.SPSR.BIT.OVRF = 0;
    // パリティエラーフラグ
    RSPI1.SPSR.BIT.PERF = 0;

    // RSPIアイドル割り込みを禁止
    RSPI1.SPCR2.BIT.SPIIE = 0;

    // SPEビットの許可
    // 割り込みの許可
    // RSPI送信割り込み要求の発生を許可
    RSPI1.SPCR.BIT.SPTIE = 1;
    // RSPI受信割り込み要求の発生を許可
    RSPI1.SPCR.BIT.SPRIE = 1;
    // RSPIを有効化
    RSPI1.SPCR.BIT.SPE = 1;

    while (RSPI1.SPCR.BIT.SPTIE) {
    }
    while (RSPI1.SPCR2.BIT.SPIIE) {
    }
    while (RSPI1.SPCR.BIT.SPRIE) {
    }
}

void Gyro::int_write() {
    if (!finished_write) {
        RSPI1.SPDR.LONG = send_data;
        // 送信割り込み禁止
        RSPI1.SPCR.BIT.SPTIE = 0;
        // アイドル割り込み許可
        RSPI1.SPCR2.BIT.SPIIE = 1;
    }

    finished_write = true;
}

void Gyro::int_idle() {
    RSPI1.SPCR.BIT.SPE = 0;
    RSPI1.SPCR2.BIT.SPIIE = 0;
}

void Gyro::int_read() {
    data_tmp = RSPI1.SPDR.LONG;
    RSPI1.SPCR.BIT.SPRIE = 0;
}

void Gyro::updateXAccel() {
    preprocess_read(0x2D);
    x_accel = 2.0 * (int16_t)data_tmp / 32767;
}

void Gyro::updateYAccel() {
    preprocess_read(0x2F);
    y_accel = 2.0 * (int16_t)data_tmp / 32767;
}

void Gyro::updateZAccel() {
    preprocess_read(0x31);
    z_accel = 2.0 * (int16_t)data_tmp / 32767;
}

void Gyro::updateXAngVel() {
    preprocess_read(0x33);
    x_ang_vel = 2000.0 * (int16_t)data_tmp / 32767 * MY_PI / 180;
}

void Gyro::updateYAngVel() {
    preprocess_read(0x35);
    y_ang_vel = 2000.0 * (int16_t)data_tmp / 32767 * MY_PI / 180;
}

void Gyro::updateZAngVel() {
    preprocess_read(0x37);
    z_ang_vel = 2000.0 * (int16_t)data_tmp / 32767 * MY_PI / 180;
}

void Gyro::update() {
    updateXAccel();
    updateYAccel();
    updateZAccel();
    updateXAngVel();
    updateYAngVel();
    updateZAngVel();
}

float Gyro::getXAccel() { return x_accel; }
float Gyro::getYAccel() { return y_accel; }
float Gyro::getZAccel() { return z_accel; }
float Gyro::getXAngVel() { return x_ang_vel; }
float Gyro::getYAngVel() { return y_ang_vel; }
float Gyro::getZAngVel() { return z_ang_vel; }
