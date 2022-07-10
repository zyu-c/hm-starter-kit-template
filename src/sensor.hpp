#pragma once

extern "C" {
#include "iodefine.h"
}

#include <cstdint>

typedef enum { left_front, left, right, right_front } sensor_dir_t;

class Sensor {
   private:
    uint8_t sensor[4];
    void initInfraredLight();
    void initAdc();

   public:
    void init();
    void update();
    void ledOn();
    void ledOff();
    uint16_t getValue(sensor_dir_t dir);
};
