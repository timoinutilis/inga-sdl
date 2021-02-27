//
// Copyright (c) 2020 Timo Kloss
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include <SDL2/SDL.h>
#ifdef __APPLE__
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_mixer/SDL_mixer.h>
#else
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#endif
#include "Config.h"
#include "Global.h"
#include "Game.h"
#include "Font.h"
#include "GameConfig.h"

#include <stdio.h>
#include <assert.h>

// SDL_TEXTINPUTEVENT_TEXT_SIZE + 1
#define MAX_CHEAT_SIZE 33

static Uint32 screen_options = SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP;

static bool parse_args(int argc, char **argv) {
    bool res = false;
    for (int i = 1; i < argc; ++i) {
        if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h")) == 0) {
            res = true;
        } else if ((strcmp(argv[i], "--window") == 0) || (strcmp(argv[i], "-w")) == 0) {
            screen_options = SDL_WINDOW_SHOWN;
        } else {
            printf("unknown argument: %s\n", argv[i]);
        }
    }
    return res;
}

static void print_help() {
    printf("usage:\n"
           "  -h, --help         show help message and quit\n"
           "  -w, --window       enable window mode\n");
}

int main(int argc, char **argv) {
    if (parse_args(argc, argv)) {
        print_help();
        exit(EXIT_SUCCESS);
    }

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();

    int flags = MIX_INIT_OGG;
    const int inited = Mix_Init(flags);
    if ((inited & flags) != flags) {
        printf("Mix_Init: %s\n", Mix_GetError());
    }

    GameConfig *config = LoadGameConfig();
    if (!config) {
        exit(EXIT_FAILURE);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    SDL_Window *window = SDL_CreateWindow(config->gameName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
                                          screen_options);
    assert(window);

#ifdef SDL2_WORKAROUND
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
#else
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
#endif
    assert(renderer);
    
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_Texture *prerenderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH,
                                                      SCREEN_HEIGHT);
    assert(prerenderTexture);
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
    }
    Mix_AllocateChannels(2);

    SDL_Event event;

    SetGlobalRenderer(renderer);

    Image *paletteImage = LoadImage(config->paletteFilename, NULL, false, true);
    if (paletteImage) {
        SetGlobalPalette(paletteImage->surface->format->palette);
    }

    Game *game = CreateGame(config);

    int mouseX = 0;
    int mouseY = 0;
    int mouseButtonIndex = 0;
    bool cheatInputActive = false;
    char cheatInput[MAX_CHEAT_SIZE];
    memset(cheatInput,0,MAX_CHEAT_SIZE);

    unsigned long lastTicks = SDL_GetTicks();

    SDL_Rect screenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    while (!ShouldQuit()) {
        Uint32 ticks = SDL_GetTicks();
        int deltaTicks = (int) (ticks - lastTicks);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    SetShouldQuit();
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
                    if (event.key.keysym.sym == SDLK_TAB) {
                        cheatInputActive = true;
                        cheatInput[0] = 0;
                    } else if (event.key.keysym.sym == SDLK_RETURN) {
                        HandleGameCheat(game, cheatInput);
                        cheatInputActive = false;
                    } else {
                        HandleKeyInGame(game, event.key.keysym);
                    }
                    break;
                case SDL_TEXTINPUT:
                    if (cheatInputActive) {
                        strncat(cheatInput, event.text.text, SDL_TEXTINPUTEVENT_TEXT_SIZE);
                    }
                    break;
            }
        }

        HandleMouseInGame(game, mouseX, mouseY, mouseButtonIndex);
        UpdateGame(game, deltaTicks);

        SDL_SetRenderTarget(renderer, prerenderTexture);
        DrawGame(game);

        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, prerenderTexture, &screenRect, &screenRect);
        SDL_RenderPresent(renderer);

        mouseButtonIndex = 0;
        lastTicks = ticks;

        // limit to 60 FPS
        Uint32 ticksDelta = SDL_GetTicks() - ticks;
        if (ticksDelta < 16) {
            SDL_Delay(16 - ticksDelta);
        }
    }

    FreeGame(game);
    FreeImage(paletteImage);
    FreeGameConfig(config);

    Mix_CloseAudio();

    SDL_DestroyTexture(prerenderTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
