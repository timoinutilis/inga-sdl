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

int main(int argc, const char * argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_Window *window = SDL_CreateWindow("Inga", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    SDL_Event event;
    int quit = 0;
    
    struct Image *backgroundImage = LoadImageIBM("game/BitMaps/HafenL.ibm", renderer, NULL, false);
    struct Image *characterImage = LoadImageIBM("game/BitMaps/ErmLaufenLinks.ibm", renderer, backgroundImage->palette, true);
    
    int mouseX = 0;
    int mouseY = 0;
    
    while (!quit) {
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
        
        SDL_RenderClear(renderer);
        
        DrawImage(backgroundImage, renderer, 0, 0);
        DrawImage(characterImage, renderer, mouseX, mouseY);
        
        SDL_RenderPresent(renderer);
    }
    
    FreeImage(characterImage);
    FreeImage(backgroundImage);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    return 0;
}
