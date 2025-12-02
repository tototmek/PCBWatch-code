#include "simulator.h"
#include <iostream>

namespace {
    void DrawPixelAt(SDL_Renderer* renderer, uint8_t value, int xIdx, int yIdx);
    void DrawDisplayData(SDL_Renderer* renderer);
    void DrawGrid(SDL_Renderer* renderer);
} // namespace

void Simulator_Init(Simulator* sim) {
    SDL_Init(SDL_INIT_VIDEO);
    sim->window = SDL_CreateWindow("PCBWatch Display Simulator",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          kImageSize, kImageSize,
                                          0);
    sim->renderer = SDL_CreateRenderer(sim->window, -1, 0);
    SDL_SetRenderDrawBlendMode(sim->renderer, SDL_BLENDMODE_BLEND);
}

void Simulator_Update(Simulator* sim) {
        SDL_SetRenderDrawColor(sim->renderer, 0, 0, 0, 255);
        SDL_RenderClear(sim->renderer);
        DrawDisplayData(sim->renderer);
        DrawGrid(sim->renderer);
        SDL_RenderPresent(sim->renderer);
    }

bool Simulator_CheckExit(Simulator* sim) {
    static SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                return true;
        }
    }
    return false;
}

void Simulator_DeInit(Simulator* sim) {
    SDL_DestroyWindow(sim->window);
    SDL_Quit();
}

namespace {

void DrawPixelAt(SDL_Renderer* renderer, uint8_t value, int xIdx, int yIdx) {
    static SDL_Rect rect = {.x=0, .y=0, .w=kPixelSize, .h=kPixelSize};
    rect.x = xIdx * kPixelSize;
    rect.y = yIdx * kPixelSize;
    uint8_t color = value * 255;
    SDL_SetRenderDrawColor(renderer, color, color, color, 255);
    SDL_RenderFillRect(renderer, &rect);
}


void DrawDisplayData(SDL_Renderer* renderer) {
    for (int j = 0; j < kNumPixels; ++j) {
        for (int i = 0; i < kNumBytes; ++i) {
            uint8_t byte = epdDisplayData[j][i];
            for (int k = 0; k < 8; ++k) {
                uint8_t value = 1 & (byte >> (7 - k));
                DrawPixelAt(renderer, value, (8*i)+k, j);
            }
        }
    }
}

void DrawGrid(SDL_Renderer* renderer) {
    for (int i = 0; i < kNumBytes; ++i) {
        int position = i*8*kPixelSize;
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 180);
        SDL_RenderDrawLine(renderer, position, 0, position, kImageSize);
        SDL_RenderDrawLine(renderer, 0, position, kImageSize, position);
    }
    for (int i = 0; i < kNumPixels; ++i) {
        int position = i*kPixelSize;
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100);
        SDL_RenderDrawLine(renderer, position, 0, position, kImageSize);
        SDL_RenderDrawLine(renderer, 0, position, kImageSize, position);
    }
}

} // namespace