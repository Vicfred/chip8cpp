#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include "chip8.h"

void chip8::init() {
    pc = 0x200; // The first 512(0x200) bytes of memory are reserved
    opcode = I = sp = 0x00;

    // Clear the stack
    memset(stack, 0, 16);

    // Clear the registers
    memset(V, 0, 16);

    // Clear the memory
    memset(memory, 0, 4096);

    // Reset the timers
    delay_timer = 0;
    sound_timer = 0;

    // Clear the screen once
    drawFlag = true;

    // Clear the display
    memset(gfx, 0, 2048);

    // Clear the keys
    memset(key, 0, 16);

    // TODO load the fontset
    /*
     * for(int i = 0; i < 80; ++i)
     * memory[i] = chip8_fontset[i];
     */

    // TODO srand(time(NULL));
}

chip8::chip8() {} // Empty
chip8::~chip8() {} // Empty

void chip8::emulateCycle() {
    // Fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];
}

bool chip8::loadRom(const char *filename) {
    init();
    printf("Loading: %s\n", filename);

    // Open file
    FILE *fp = fopen(filename, "rb");
    if(NULL == fp) {
        fputs("File error", stderr);
        return false;
    }

    // Check file size
    fseek(fp, 0, SEEK_END);
    uint64_t lSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    printf("Filesize: %d\n", (int)lSize);

    // Allocate memory to contain the whole file
    uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * lSize);
    if(NULL == buffer) {
        fputs("Memory error", stderr);
        return false;
    }

    // Copy the file into the buffer
    size_t result = fread(buffer, 1, lSize, fp);
    if(result != lSize) {
        fputs("Reading error", stderr);
        return false;
    }

    // Copy the buffer to Chip8 memory
    // The first 512 bytes of memory are reserved
    if(lSize < (4096 - 512)) {
        for(int i = 0; i < lSize; ++i)
            memory[i + 512] = buffer[i];
    } else
        printf("Error: ROM too big for memory\n");

    // Close the file, free the buffer
    fclose(fp);
    free(buffer);

    return true;
}

void chip8::debugMemory() {
    for(int idx = 1; idx <= 4096; ++idx) {
        printf("%02x\t", memory[idx-1]);
        if(idx % 32 == 0)
            printf("\n");
    }
}
