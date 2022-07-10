#include <cstdint>

#include "control.hpp"
#include "delay.hpp"

extern "C" {
#include "spi.h"
}

extern "C" {
__attribute__((interrupt)) void INT_Excep_CMT0_CMI0(void) { delay_cnt++; }

__attribute__((interrupt)) void INT_Excep_CMT1_CMI1(void) { control(); }

__attribute__((interrupt)) void INT_Excep_RSPI0_SPRI0(void) { read_spdr_enc(); }

__attribute__((interrupt)) void INT_Excep_RSPI0_SPTI0(void) {
    write_spdr_enc();
}

__attribute__((interrupt)) void INT_Excep_RSPI0_SPII0(void) { spii_int_enc(); }

__attribute__((interrupt)) void INT_Excep_RSPI1_SPRI1(void) {
    read_spdr_gyro();
}

__attribute__((interrupt)) void INT_Excep_RSPI1_SPTI1(void) {
    write_spdr_gyro();
}

__attribute__((interrupt)) void INT_Excep_RSPI1_SPII1(void) { spii_int_gyro(); }
}
