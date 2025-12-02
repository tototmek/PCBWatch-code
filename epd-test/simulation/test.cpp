#include <SDL2/SDL.h>
#include <iostream>

constexpr int kPixelSize = 5;
constexpr int kNumPixels = 200;
constexpr int kImageSize = kPixelSize*kNumPixels;
constexpr int kNumBytes = kNumPixels / 8;


uint8_t displayData[kNumPixels][kNumBytes] = {0};

void DrawDisplayData(SDL_Renderer* renderer);
void DrawGrid(SDL_Renderer* renderer);

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("PCBWatch Display Simulator",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          kImageSize, kImageSize,
                                          0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    displayData[0][4] = 0xff;
    displayData[1][4] = 0x7e;

    bool running = true;
    SDL_Event event;
    while(running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        DrawDisplayData(renderer);
        DrawGrid(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(30);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
}

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
            uint8_t byte = displayData[j][i];
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