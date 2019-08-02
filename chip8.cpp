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

    uint16_t    nnn = opcode & 0x0FFF; // lowest 12 bits

    uint8_t n   = opcode & 0x000F; // lowest 4 bits
    uint8_t kk  = opcode & 0x00FF; // lowest 8 bits
    uint8_t x   = opcode >> 8 & 0x000F; // lower 4 bits of the high byte
    uint8_t y   = opcode >> 4 & 0x000F; // upper 4 bits of the low byte

    // Process the opcode
    switch(opcode & 0xF000) {
        case 0x000:
            switch(n) {
                case 0x0000:
                    // Clears the screen
                    printf("CLS\n");
                    for(int i = 0; i < 2048; ++i)
                        gfx[i] = 0x0;
                    drawFlag = true;
                    pc += 2;
                break;

                case 0x000E:
                    // Returns from subroutine
                    printf("RET\n");
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                break;

                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
        break;

        case 0x1000:
            // Jump to location nnn
            printf("JP %X\n", nnn);
            pc = nnn;
        break;

        case 0x2000:
            // Call subroutine at nnn
            printf("CALL %X\n", nnn);
            stack[sp] = pc; // Store the current address in stack
            ++sp;           // Increment stack pointer
            pc = nnn;       // Set the program counter to the address at nnn
        break;

        case 0x3000:
            // Skip next instruction if Vx == kk
            printf("SE V%X, %X\n", x, kk);
            if(V[x] == kk)
                pc += 4;
            else
                pc += 2;
        break;

        case 0x4000:
            // Skip next instruction if Vx != kk
            printf("SNE V%X, %X\n", x, kk);
            if(V[x] != kk)
                pc += 4;
            else
                pc += 2;
        break;

        case 0x5000:
            // Skip next instruction if Vx == Vy
            printf("SE V%X, V%X\n", x, y);
            if(V[x] == V[y])
                pc += 4;
            else
                pc += 2;
        break;

        case 0x6000:
            // Set Vx = kk
            printf("LD V%X, %X\n", x, kk);
            V[x] = kk;
            pc += 2;
        break;

        case 0x7000:
            // Set Vx = Vx + kk
            printf("LD V%X, %X\n", x, kk);
            V[x] += kk;
            pc += 2;
        break;

        case 0x8000:
            switch(n) {
                case 0x0000:
                    // Set Vx = Vy
                    printf(" LD V%X, V%X\n", x, y);
                    V[x] = V[y];
                    pc += 2;
                break;

                case 0x0001:
                    // Set Vx = Vx OR Vy
                    printf("OR V%X, V%X\n", x, y);
                    V[x] |= V[y];
                    pc += 2;
                break;

                case 0x0002:
                    // Set Vx = Vx AND Vy
                    printf("AND V%X, V%X\n", x, y);
                    V[x] &= V[y];
                    pc += 2;
                break;

                case 0x0003:
                    // Set Vx = Vx XOR Vy
                    printf("XOR V%X, V%X\n", x, y);
                    V[x] ^= V[y];
                    pc += 2;
                break;

                case 0x0004:
                    // Set Vx = Vx + Vy, set VF = carry
                    printf("ADD V%X, V%X\n", x, y);
                    if(V[y] > (0xFF - V[x]))
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[x] += V[y];
                    pc += 2;
                break;

                case 0x0005:
                    // Set Vx = Vx - Vy, set VF = NOT borrow
                    printf("SUB V%X, V%X\n", x, y);
                    if(V[y] > V[x])
                        V[0xF] = 0; // There is borrow
                    else
                        V[0xF] = 1;
                    V[x] -= V[y];
                    pc += 2;
                break;

                case 0x0006:
                    // Shifts Vx right by one.
                    // VF is set to the value of the LSB of Vx before the shift
                    printf("SHR V%X\n", x);
                    V[0xF] = V[x] & 0x1;
                    V[x] >>= 1;
                    pc += 2;
                break;

                case 0x0007:
                    // Set Vx = Vy - Vx, set VF = NOT borrow
                    printf("SUBN V%X, V%X\n", x, y);
                    if(V[x] > V[y])
                        V[0xF] = 0; // There is a borrow
                    else
                        V[0xF] = 1;
                    V[x] = V[y] - V[x];
                    pc += 2;
                break;

                case 0x000E:
                    // Shifts Vx left by one.
                    // VF is set to the value of the MSB of Vx before the shift
                    printf("SHL V%X\n", x);
                    V[0xF] = V[x] >> 7;
                    V[x] <<= 1;
                    pc += 2;
                break;

                default:
                    printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
            }
        break;

        case 0x9000:
            // Skip next instruction if Vx != Vy
            printf("SNE V%X, V%X\n", x, y);
            if(V[x] != V[y])
                pc += 4;
            else
                pc += 2;
        break;

        case 0xA000:
            // Set I = nnn
            printf("LD I, %X\n", nnn);
            I = nnn;
            pc += 2;
        break;

        case 0xB000:
            // Jump to location nnn + V0
            printf("JP V0, nnn\n");
            pc = V[0x0] = nnn;
        break;

        case 0xC000:
            // Sets Vx to a random number AND kk
            printf("RND V%X, %X\n", x, kk);
            V[x] = (rand() % 0xFF) & kk;
            pc += 2;
        break;

        case 0xD000:
            // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
            // Each row of 8 pixels is read as bit-coded starting from memory location I;
            // I value doesn't change after the execution of this instruction.
            // VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
            // and to 0 if that doesn't happen
            printf("DRW V%X, V%X, %X\n", x, y, n);
            uint8_t pixel;
            V[0xF] = 0;
            for(int yline = 0; yline < n; yline++) {
                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++) {
                    if(0 != (pixel & (0x80 >> xline))) {
                        if(1 == gfx[x + xline + ((y + yline) * 64)])
                            V[0xF] = 1;
                        gfx[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }
            drawFlag = true;
            pc += 2;
        break;

        case 0xE000:
            switch(kk) {
                case 0x009E:
                    // Skip next instruction if key with the value of Vx is pressed
                    printf("SKP V%X\n", x);
                    if(0 != key[V[x]])
                    pc += 4;
                    else
                    pc += 2;
                break;

                case 0x00A1:
                    // Skip next instruction if key with the value of Vx isn't pressed
                    printf("SKP V%X\n", x);
                    if(0 == key[V[x]])
                        pc += 4;
                    else
                        pc += 2;
                break;

                default:
                    printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
            }
        break;

        case 0xF000:
            switch(kk) {
                case 0x0007:
                    // Set Vx = delay timer value
                    printf("LD V%X, DT\n", x);
                    pc += 2;
                break;

                case 0x000A: {
                    // Wait for a key press, store the value of the key in Vx
                    printf("LD V%X, K\n", x);
                    bool keyPress = false;
                    for(int i = 0; i < 16; ++i) {
                        if(0 != key[i]) {
                            V[x] = i;
                            keyPress = true;
                        }
                    }
                    // If we didn't received a keypress, skip this cycle and try again
                    if(!keyPress)
                        return;
                    pc += 2;
                }
                break;

                case 0x0015:
                    // Set delay timer = Vx.
                    printf("LD DT, V%X\n", x);
                    delay_timer = V[x];
                    pc += 2;
                break;

                case 0x0018:
                    //  Set sound timer = Vx
                    printf("LD ST, V%X\n", x);
                    sound_timer = V[x];
                    pc += 2;
                break;

                case 0x001E:
                    // Set I = I + Vx
                    printf("ADD I, V%X\n", x);
                    if(I + V[x] > 0xFFF) // VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    I += V[x];
                    pc += 2;
                break;

                case 0x0029:
                    // Set I = location of sprite for digit Vx
                    // Characters 0-F (in hexadecimal) are represented by a 4x5 font
                    printf("LD F, V%X\n", x);
                    I = V[x] * 0x5;
                    pc += 2;
                break;

                case 0x0033:
                    // Store BCD representation of Vx in memory locations I, I+1, and I+2
                    printf("LD B, V%X\n", x);
                    memory[I] = V[x]/10;
                    memory[I+1] = (V[x]/10)%10;
                    memory[I+2] = (V[x]%100)%10;
                    pc += 2;
                break;

                case 0x0055:
                    // Store registers V0 through Vx in memory starting at location I
                    printf("LD [I], V%X\n", x);
                    for(int i = 0; i <= x; ++i)
                        memory[I+i] = V[i];
                    // On the original interpreter, when the operation is done, I = I + X + 1
                    I += x + 1;
                    pc += 2;
                break;

                case 0x0065:
                    // Read registers V0 through Vx from memory starting at location I
                    printf("LD V%X, [I]", x);
                    for(int i = 0; i <= x; ++i)
                        V[i] = memory[I+i];
                    // On the original interpreter, when the operation is done, I = I + X + 1
                    I += x + 1;
                    pc += 2;
                break;

                default:
                    printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
            }
        break;

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
    }

    // Update timers
    if(delay_timer > 0)
        --delay_timer;
    if(sound_timer > 0) {
        if(1 == sound_timer)
            printf("BEEP\n");
        --sound_timer;
    }
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
