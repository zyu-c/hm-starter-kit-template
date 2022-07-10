#pragma once

extern "C" {
#include "iodefine.h"
}

#include <cstdint>

typedef enum { left_front, left, right, right_front } sensor_dir_t;

class Sensor {
   private:
    uint16_t sensor[4];
    uint16_t battery_voltage;

    void initInfraredLight();
    void initAdc();

   public:
    void init();
    void update();
    void ledOn();
    void ledOff();
    uint16_t getSensorValue(sensor_dir_t dir);
    void updateBatteryVoltage();
    uint16_t getBatteryVoltage();
    bool isLowBattery();
};
