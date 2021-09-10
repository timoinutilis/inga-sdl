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

#include "Global.h"

SDL_Renderer *globalRenderer = NULL;
SDL_Palette *globalPalette = NULL;
bool shouldQuit = false;

void SetGlobalRenderer(SDL_Renderer *renderer) {
    globalRenderer = renderer;
}

SDL_Renderer *GetGlobalRenderer() {
    return globalRenderer;
}

void SetGlobalPalette(SDL_Palette *palette) {
    globalPalette = palette;
}

SDL_Palette *GetGlobalPalette(void) {
    return globalPalette;
}

void SetShouldQuit(void) {
    shouldQuit = true;
}

bool ShouldQuit(void) {
    return shouldQuit;
}

bool CanHover(ButtonState buttonState) {
#ifdef TOUCH
    return buttonState != ButtonStateIdle;
#else
    return true;
#endif
}

ButtonState SelectionButtonState(void) {
#ifdef TOUCH
    return ButtonStateRelease;
#else
    return ButtonStateClickLeft;
#endif
}

int FocusOffset(void) {
#ifdef TOUCH
    return 64;
#else
    return 4;
#endif
}
