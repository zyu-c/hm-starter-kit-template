#include <cstdint>

#include "control.hpp"
#include "delay.hpp"
#include "encoder.hpp"
#include "gyro.hpp"

extern "C" {
__attribute__((interrupt)) void INT_Excep_CMT0_CMI0(void) { delay_cnt++; }

__attribute__((interrupt)) void INT_Excep_CMT1_CMI1(void) { control(); }

__attribute__((interrupt)) void INT_Excep_RSPI0_SPRI0(void) {
    Encoder::getInstance()->int_read();
}
__attribute__((interrupt)) void INT_Excep_RSPI0_SPTI0(void) {
    Encoder::getInstance()->int_write();
}
__attribute__((interrupt)) void INT_Excep_RSPI0_SPII0(void) {
    Encoder::getInstance()->int_idle();
}

__attribute__((interrupt)) void INT_Excep_RSPI1_SPRI1(void) {
    Gyro::getInstance()->int_read();
}
__attribute__((interrupt)) void INT_Excep_RSPI1_SPTI1(void) {
    Gyro::getInstance()->int_write();
}
__attribute__((interrupt)) void INT_Excep_RSPI1_SPII1(void) {
    Gyro::getInstance()->int_idle();
}
}
