#include "mouse.hpp"

int main() {
    Mouse* mouse = Mouse::getInstance();
    mouse->init();
    mouse->startControllerTimer();

    while (1) {
        mouse->ioex->set(0x00);
        delay_ms(100);
        mouse->ioex->set(0x0F);
        delay_ms(100);
    }
    return 0;
}
