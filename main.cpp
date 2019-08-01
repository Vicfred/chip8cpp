#include <iostream>
#include <cstdint>

class chip8 {
    uint16_t pc;            // Program counter
    uint16_t opcode;        // Current opcode
    uint16_t sp;            // Stack pointer
    uint16_t I;             // Index register

    uint8_t     V[16];          // V registers (V0-VF)
    uint8_t     memory[4096];   // Memory (size = 4k)
    uint16_t    stack[16];      // Stack (16 levels)

    uint8_t delay_timer;
    uint8_t sound_timer;
};

int main() {
    std::cout << "olis olis" << std::endl;
    return 0;
}
