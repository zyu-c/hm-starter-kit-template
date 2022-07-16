#pragma once

#include <cstdint>

class Ioex {
   private:
    uint8_t led_state;
    uint8_t button_state;

    void send();

    static Ioex* ioex_p;

   public:
    static Ioex* getInstance();

    void on(uint8_t port);
    void off(uint8_t port);
    void set(uint8_t port, bool state);
    void set(uint8_t state);
    void toggle(uint8_t port);

    void updateButton();
    uint8_t getState();
    bool getState(uint8_t port);
};
