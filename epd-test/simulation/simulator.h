#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <SDL2/SDL.h>
#include "epd_emulator.h"

constexpr int kPixelSize = 5;
constexpr int kImageSize = kPixelSize*kNumPixels;

struct Simulator {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};

void Simulator_Init(Simulator* sim);
void Simulator_Update(Simulator* sim); 
bool Simulator_CheckExit(Simulator* sim);
void Simulator_DeInit(Simulator* sim);


#endif // SIMULATOR_H