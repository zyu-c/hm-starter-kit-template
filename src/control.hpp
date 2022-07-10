#pragma once

#include <cstdint>

extern "C" {
#include "iodefine.h"
#include "sci.h"
}

void initControllerTimer();
void startControllerTimer();
void stopControllerTimer();

void control();
