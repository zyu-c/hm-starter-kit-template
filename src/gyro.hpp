#pragma once

#include <cstdint>

class Gyro {
   private:
    static Gyro* gyro_p;

    uint32_t send_data;
    uint16_t data_tmp;
    bool finished_write;

    float x_accel;
    float y_accel;
    float z_accel;
    float x_ang_vel;
    float y_ang_vel;
    float z_ang_vel;

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

    float getXAccel();
    float getYAccel();
    float getZAccel();
    float getXAngVel();
    float getYAngVel();
    float getZAngVel();
};
