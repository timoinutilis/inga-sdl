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
#include <SDL2_ttf/SDL_ttf.h>
#include "Config.h"
#include "Game.h"
#include "Font.h"

int main(int argc, const char * argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    
    SDL_Window *window = SDL_CreateWindow("Inga", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    SDL_Event event;
    int quit = 0;
    
    Game *game = CreateGame(renderer);
        
    int mouseX = 0;
    int mouseY = 0;
    bool mouseClick = false;
    
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
                case SDL_MOUSEBUTTONDOWN:
                    mouseClick = true;
                    break;
            }
        }
        
        if (mouseClick) {
            ElementMoveTo(game->mainPerson, mouseX, mouseY, 2);
        }
        
        UpdateGame(game, deltaTicks);
        
        SDL_RenderClear(renderer);
        DrawGame(game, renderer);
        SDL_RenderPresent(renderer);
                
        mouseClick = false;
        lastTicks = ticks;
    }
    
    FreeGame(game);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}
