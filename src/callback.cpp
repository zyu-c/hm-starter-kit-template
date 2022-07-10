#include <cstdint>

#include "delay.hpp"

extern "C" {
__attribute__((interrupt)) void INT_Excep_CMT0_CMI0(void) { delay_cnt++; }
}
