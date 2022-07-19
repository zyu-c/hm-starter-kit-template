#pragma once

#include <cstdint>

class Motor {
   private:
    static Motor* motor_p;

   public:
    static Motor* getInstance();

    void init();

    void setDuty(uint8_t port, float duty);
};
