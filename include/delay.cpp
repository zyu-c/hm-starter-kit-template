#include "delay.hpp"

uint16_t delay_cnt;

void initDelayTimer() {
    // CMT0有効化
    SYSTEM.PRCR.WORD = 0xA502;
    MSTP(CMT0) = 0;
    SYSTEM.PRCR.WORD = 0xA500;

    // 割り込みを許可
    CMT0.CMCR.BIT.CMIE = 1;
    ICU.IR[28].BIT.IR = 0;
    ICU.IPR[4].BIT.IPR = 13;
    ICU.IER[3].BIT.IEN4 = 1;

    // 128分周
    // 375Clkで1ms
    CMT0.CMCR.BIT.CKS = 2;

    // 1msごとに割り込み
    CMT0.CMCOR = 375 - 1;
}

void delay_ms(uint16_t time) {
    CMT0.CMCNT = 0;
    delay_cnt = 0;
    CMT.CMSTR0.BIT.STR0 = 1;
    while (delay_cnt < time) {
    }
    CMT.CMSTR0.BIT.STR0 = 0;
}
