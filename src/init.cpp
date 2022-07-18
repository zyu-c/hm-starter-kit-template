#include "init.hpp"

void initClock() {
    //プロテクト解除
    SYSTEM.PRCR.WORD = 0xA50b;

    // 分周器設定
    // 12MHz -> 192MHz
    SYSTEM.PLLCR.WORD = 0x0F00;

    // PLL有効化
    SYSTEM.PLLCR2.BYTE = 0x00;

    // PLL安定まで待機
    SYSTEM.PLLWTCR.BYTE = 0x0F;

    // 周辺回路動作周波数設定
    // FCLK : 1/4（FlashIF，最大50MHz）
    // ICLK : 1/2（システムクロック，最大100MHz）
    // BCLK : 停止（外部バスクロック）
    // SDCLK : 停止（SDRAMクロック）
    // PCLKA : 1/2（周辺モジュールクロックA，最大100MHz）
    // PCLKB = PCLK : 1/4（周辺モジュールクロックB，最大50MHz）
    // UCLK : 1/4（USBクロック）
    // IECLK : 1/4（IEBUSクロック）
    // クロックソース：PLL
    SYSTEM.SCKCR.LONG = 0x21C21211;
    SYSTEM.SCKCR2.WORD = 0x0032;
    SYSTEM.BCKCR.BYTE = 0x01;
    SYSTEM.SCKCR3.WORD = 0x0400;

    // 再プロテクト
    SYSTEM.PRCR.WORD = 0xA500;
}

void init() {
    initClock();
    init_sci();
}
