#pragma once

#include <cstdint>

class Gyro {
   private:
    static Gyro* gyro_p;

    uint32_t send_data;
    uint16_t data_tmp;
    bool finished_write;

    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t x_ang_vel;
    int16_t y_ang_vel;
    int16_t z_ang_vel;

    void updateXAccel();
    void updateYAccel();
    void updateZAccel();
    void updateXAngVel();
    void updateYAngVel();
    void updateZAngVel();

    void setupSlave();
    void preprocess_write(uint8_t address, uint8_t data);
    void preprocess_read(uint8_t address);

   public:
    static Gyro* getInstance();

    void init();
    void update();

    void write();
    void idle();
    void read();

    int16_t getXAccel();
    int16_t getYAccel();
    int16_t getZAccel();
    int16_t getXAngVel();
    int16_t getYAngVel();
    int16_t getZAngVel();
};
