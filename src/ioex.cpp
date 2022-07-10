#include "ioex.hpp"

void Ioex::send() {
    uint8_t a = 0x00;
    a |= (led_state & 0x08) >> 1;
    a |= (led_state & 0x04) << 1;
    a |= (led_state & 0x02) >> 1;
    a |= (led_state & 0x01) << 1;
    a = ~a;
    IOex_LED(a);
}

void Ioex::on(uint8_t port) {
    led_state |= 0x01 << port;
    send();
}

void Ioex::off(uint8_t port) {
    led_state &= ~(0x01 << port);
    send();
}

void Ioex::set(uint8_t port, bool state) {
    led_state = (led_state & ~(0x01 << port)) | ((state ? 0x01 : 0x00) << port);
    send();
}

void Ioex::set(uint8_t state) {
    led_state = state;
    send();
}

void Ioex::toggle(uint8_t port) {
    led_state ^= 0x01 << port;
    send();
}

void Ioex::updateButton() { button_state = IOex_SWITCH() >> 4; }

uint8_t Ioex::getState() { return button_state; }

bool Ioex::getState(uint8_t port) { return button_state & (0x01 << port); }
