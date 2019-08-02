#include <cstdio>
#include "chip8.h"

chip8 momoko;

int main(int argc, char **argv) {
    if(argc < 2) {
        printf("Usage: chip8 romname");
        return 1;
    }

    // Load game
    if(!momoko.loadRom(argv[1]))
        return 1;
    momoko.debugMemory();
    momoko.debugStack();
    return 0;
}
