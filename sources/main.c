//
// Copyright 2020 Timo Kloss
//
// This file is part of Inga.
//
// Inga is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Inga is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with LowRes NX.  If not, see <http://www.gnu.org/licenses/>.
//

#include <SDL2/SDL.h>
#include "Image.h"
#include "Location.h"

int main(int argc, const char * argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_Window *window = SDL_CreateWindow("Inga", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    SDL_Event event;
    int quit = 0;
    
    struct Location *location = CreateLocation(1, "HafenL", renderer);
    
    struct Element *deco1 = CreateElement(1);
    deco1->image = LoadImageIBM("HafenWasserL", renderer, location->image->palette, false);
    deco1->x = 210;
    deco1->y = 440;
    AddElement(location, deco1);
    
    struct Element *deco2 = CreateElement(2);
    deco2->image = LoadImageIBM("HafenWasserLa", renderer, location->image->palette, false);
    deco2->x = 0;
    deco2->y = 450;
    AddElement(location, deco2);
    
    struct Element *deco3 = CreateElement(3);
    deco3->image = LoadImageIBM("ErmLaufenLinks", renderer, location->image->palette, true);
    deco3->x = 360;
    deco3->y = 380;
    AddElement(location, deco3);
    
    int mouseX = 0;
    int mouseY = 0;
    
    unsigned long lastTicks = SDL_GetTicks();
    
    while (!quit) {
        unsigned long ticks = SDL_GetTicks();
        int deltaTicks = (int)(ticks - lastTicks);
        
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_MOUSEMOTION:
                    mouseX = event.motion.x;
                    mouseY = event.motion.y;
                    break;
            }
        }
        
        UpdateLocation(location, deltaTicks);
        
        SDL_RenderClear(renderer);
        DrawLocation(location, renderer);
        SDL_RenderPresent(renderer);
        
        lastTicks = ticks;
    }
    
    FreeLocation(location);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    return 0;
}
