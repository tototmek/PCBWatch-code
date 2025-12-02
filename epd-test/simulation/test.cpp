#include <iostream>
#include "simulator.h"

int main()
{
    Simulator sim;
    Simulator_Init(&sim);

    Simulator_Update(&sim);

    while(true) {
        if (Simulator_CheckExit(&sim)) {
            break;
        }
        SDL_Delay(30);
    }

    Simulator_DeInit(&sim);
}
