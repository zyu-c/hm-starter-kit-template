#include "iodefine.h"

#ifdef CPPAPP
// Initialize global constructors
extern void __main() {
    static int initialized;
    if (!initialized) {
        typedef void (*pfunc)();
        extern pfunc __ctors[];
        extern pfunc __ctors_end[];
        pfunc *p;

        initialized = 1;
        for (p = __ctors_end; p > __ctors;) (*--p)();
    }
}
#endif

int main() {
    while (1) {
    }
    return 0;
}
