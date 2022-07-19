#pragma once

#include <cstdint>

class Ioex {
   private:
    static Ioex* ioex_p;

    void setupSlave();

    void write(uint8_t data[], uint8_t size);
    uint8_t read();

    uint8_t led_state;
    uint8_t button_state;

   public:
    static Ioex* getInstance();

    void init();

    void update();

    void on(uint8_t port);
    void off(uint8_t port);
    void toggle(uint8_t port);
    void set(uint8_t state);

    bool getState(uint8_t port);
};
