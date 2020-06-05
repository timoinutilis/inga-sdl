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

#include "Fader.h"
#include "Config.h"
#include <SDL2/SDL.h>
#include "Global.h"

void UpdateFader(Fader *fader, int deltaTicks) {
    if (!fader) return;
    switch (fader->state) {
        case FaderStateClose:
            break;
        case FaderStateFadingIn:
            fader->ticks += deltaTicks;
            if (fader->ticks >= FADE_DURATION) {
                fader->state = FaderStateOpen;
            }
            break;
        case FaderStateOpen:
            break;
        case FaderStateFadingOut:
            fader->ticks += deltaTicks;
            if (fader->ticks >= FADE_DURATION) {
                fader->state = FaderStateClose;
            }
            break;
    }
}

void DrawFader(Fader *fader) {
    if (!fader || fader->state == FaderStateOpen) return;
    SDL_Rect rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_SetRenderDrawBlendMode(GetGlobalRenderer(), SDL_BLENDMODE_BLEND);
    switch (fader->state) {
        case FaderStateClose:
            SDL_SetRenderDrawColor(GetGlobalRenderer(), 0, 0, 0, 0);
            break;
        case FaderStateFadingIn:
            SDL_SetRenderDrawColor(GetGlobalRenderer(), 0, 0, 0, 255 - (fader->ticks * 255 / FADE_DURATION));
            break;
        case FaderStateOpen:
            // not entering
            break;
        case FaderStateFadingOut:
            SDL_SetRenderDrawColor(GetGlobalRenderer(), 0, 0, 0, fader->ticks * 255 / FADE_DURATION);
            break;
    }
    SDL_RenderFillRect(GetGlobalRenderer(), &rect);
}

void FadeIn(Fader *fader) {
    if (!fader) return;
    fader->state = FaderStateFadingIn;
    fader->ticks = 0;
}

void FadeOut(Fader *fader) {
    if (!fader) return;
    fader->state = FaderStateFadingOut;
    fader->ticks = 0;
}
