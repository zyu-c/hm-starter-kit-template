#include "control.hpp"

void initControllerTimer() {
    // CMT1有効化
    SYSTEM.PRCR.WORD = 0xA502;
    MSTP(CMT1) = 0;
    SYSTEM.PRCR.WORD = 0xA500;

    // 割り込みを許可
    CMT1.CMCR.BIT.CMIE = 1;
    ICU.IR[29].BIT.IR = 0;
    ICU.IPR[5].BIT.IPR = 13;
    ICU.IER[3].BIT.IEN5 = 1;

    // 128分周
    // 375Clkで1ms
    CMT1.CMCR.BIT.CKS = 2;

    // 1msごとに割り込み
    // 制御周期 1kHz
    CMT1.CMCOR = 375 - 1;
}

void startControllerTimer() {
    CMT1.CMCNT = 0;
    CMT.CMSTR0.BIT.STR1 = 1;
}

void stopControllerTimer() { CMT.CMSTR0.BIT.STR1 = 0; }

void control() {}
