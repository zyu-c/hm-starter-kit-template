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
#include "sci.h"
}

class Mouse {
   private:
    static Mouse* mouse_p;

    void initClock();
    void initPeripheral();
    void initControllerTimer();
    void checkBattery();

   public:
    Buzzer* buzzer;
    Encoder* encoder;
    Gyro* gyro;
    Ioex* ioex;
    Motor* motor;
    Sensor* sensor;

    static Mouse* getInstance();
    void init();

    void control();

    void startControllerTimer();
    void stopControllerTimer();
};
