#include "mouse.hpp"

int main() {
    Mouse mouse;

    while (1) {
        mouse.ioex->set(0x00);
        mouse.ioex->update();
        delay_ms(100);
        mouse.ioex->set(0x0F);
        mouse.ioex->update();
        delay_ms(100);
    }
    return 0;
}
