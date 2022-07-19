#pragma once

#include <cstdint>

class Buzzer {
   private:
    static Buzzer* buzzer_p;

    float frequency;

   public:
    static Buzzer* getInstance();
    void init();

    void setFreq(float freq);
    void on();
    void off();
};
