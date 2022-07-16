#pragma once

#include <cstdint>

class Encoder {
   private:
    static Encoder* encoder_p;

    uint16_t data_tmp;
    uint16_t position[2];
    bool finished_write;

    void preprocess();

   public:
    static Encoder* getInstance();

    void init();

    void write();
    void idle();
    void read();

    void update();
    uint16_t getPosition(uint8_t port);
};
