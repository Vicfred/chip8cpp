#include <cstdio>
#include <SDL2/SDL.h>
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
    //momoko.debugMemory();
    //momoko.debugStack();

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("CHIP-8 Emulator",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        1280, 640,
                                        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );

    printf("SDL Error: %s\n", SDL_GetError());

    SDL_Renderer *rnd = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(rnd, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(rnd);
    SDL_RenderPresent(rnd);

    SDL_Delay(10000);

    SDL_DestroyRenderer(rnd);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
