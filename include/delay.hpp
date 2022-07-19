#pragma once

#include <cstdint>

extern "C" {
#include "iodefine.h"
}

extern uint16_t delay_cnt;

void initDelayTimer();
void delay_ms(uint16_t time);
