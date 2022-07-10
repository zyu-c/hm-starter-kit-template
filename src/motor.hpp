#pragma once

extern "C" {
#include "iodefine.h"
}

#include <cstdint>

class Motor {
   private:
   public:
    void init();
    void setDuty(uint8_t port, float duty);
};
