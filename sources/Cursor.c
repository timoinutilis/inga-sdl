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

#include "Cursor.h"
#include "Utils.h"

SDL_Cursor *LoadCursor(const char *filename) {
    SDL_Cursor *cursor = NULL;
    
    char path[FILENAME_MAX];
    GameFilePath(path, NULL, filename, "bmp");
    
    SDL_Surface *surface = SDL_LoadBMP(path);
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadCursor: %s\n", SDL_GetError());
    } else {
        cursor = SDL_CreateColorCursor(surface, 0, 0);
        SDL_FreeSurface(surface);
        if (!cursor) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadCursor: %s\n", SDL_GetError());
        }
    }
    return cursor;
}

void FreeCursor(SDL_Cursor *cursor) {
    if (!cursor) return;
    SDL_FreeCursor(cursor);
}

void SetCursor(SDL_Cursor *cursor) {
    if (!cursor) return;
    SDL_SetCursor(cursor);
    SDL_ShowCursor(SDL_ENABLE);
}

void HideCursor() {
    SDL_ShowCursor(SDL_DISABLE);
}
