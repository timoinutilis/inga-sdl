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
// along with Inga.  If not, see <http://www.gnu.org/licenses/>.
//

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include "Config.h"
#include "Global.h"
#include "Game.h"
#include "Font.h"

int main(int argc, const char * argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    
    SDL_Window *window = SDL_CreateWindow("Inga", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
//    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    SDL_Event event;
    int quit = 0;
    
    SetGlobalRenderer(renderer);
    
    Image *paletteImage = LoadImage("Thronsaal", NULL, false, true);
    SetGlobalPalette(paletteImage->surface->format->palette);
    
    Game *game = CreateGame();
        
    int mouseX = 0;
    int mouseY = 0;
    int mouseButtonIndex = 0;
    
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
                    if (mouseX < 0) mouseX = 0;
                    if (mouseY < 0) mouseY = 0;
                    if (mouseX >= SCREEN_WIDTH) mouseX = SCREEN_WIDTH - 1;
                    if (mouseY >= SCREEN_HEIGHT) mouseY = SCREEN_HEIGHT - 1;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mouseButtonIndex = event.button.button;
                    break;
                case SDL_KEYDOWN:
                    HandleKeyInGame(game, event.key.keysym);
                    break;
            }
        }
                
        HandleMouseInGame(game, mouseX, mouseY, mouseButtonIndex);
        UpdateGame(game, deltaTicks);
        
        SDL_RenderClear(renderer);
        DrawGame(game);
        SDL_RenderPresent(renderer);
                
        mouseButtonIndex = 0;
        lastTicks = ticks;
    }
    
    FreeGame(game);
    FreeImage(paletteImage);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}
