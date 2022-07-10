#pragma once

extern "C" {
#include "iodefine.h"
}

#include <cstdint>

class Buzzer {
   private:
    float frequency;

   public:
    void init();
    void setFreq(float freq);
    void on();
    void off();
};
