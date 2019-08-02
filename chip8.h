#include <cstdint>

class chip8 {
public:
    chip8();
    ~chip8();

    bool drawFlag;

    void emulateCycle();
    void debugRender();
    void debugMemory();
    bool loadRom(const char *filename);

    uint8_t gfx[64*32];     // Total amount of pixels: 2048
    uint8_t key[16];

private:
    uint16_t pc;            // Program counter
    uint16_t opcode;        // Current opcode
    uint16_t sp;            // Stack pointer
    uint16_t I;             // Index register

    uint8_t     V[16];          // V registers (V0-VF)
    uint8_t     memory[4096];   // Memory (size = 4k)
    uint16_t    stack[16];      // Stack (16 levels)

    uint8_t delay_timer;
    uint8_t sound_timer;

    void init();
};
