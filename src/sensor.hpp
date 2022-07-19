#pragma once

#include <cstdint>

typedef enum { left_front, left, right, right_front } sensor_dir_t;

class Sensor {
   private:
    static Sensor* sensor_p;

    uint16_t sensor[4];
    uint16_t battery_voltage;

    void initInfraredLight();
    void initAdc();

   public:
    static Sensor* getInstance();

    void init();
    void update();
    void updateBatteryVoltage();

    void ledOn();
    void ledOff();

    uint16_t getSensorValue(sensor_dir_t dir);
    uint16_t getBatteryVoltage();

    bool isLowBattery();
};
