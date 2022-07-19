#pragma once

#include <cstdint>

#include "buzzer.hpp"
#include "delay.hpp"
#include "encoder.hpp"
#include "gyro.hpp"
#include "ioex.hpp"
#include "motor.hpp"
#include "sensor.hpp"

extern "C" {
#include "iodefine.h"
#include "sci.h"
}

class Mouse {
   private:
    void initClock();
    void initPeripheral();
    void checkBattery();

   public:
    Buzzer* buzzer;
    Encoder* encoder;
    Gyro* gyro;
    Ioex* ioex;
    Motor* motor;
    Sensor* sensor;

    Mouse();
};
