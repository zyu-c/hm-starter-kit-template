#pragma once

#include <cstdint>

class Gyro {
   private:
    static Gyro* gyro_p;

    uint32_t send_data;
    uint16_t data_tmp;
    bool finished_write;

   public:
    static Gyro* getInstance();

    void init();

    void preprocess_write(uint8_t address, uint8_t data);
    void preprocess_read(uint8_t address);

    void write();
    void idle();
    void read();

    uint16_t getData();
};
