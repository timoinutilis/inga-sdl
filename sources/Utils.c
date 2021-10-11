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

#include "Utils.h"

Vector MakeVector(float x, float y) {
    Vector vector = {x, y};
    return vector;
}

SDL_Rect MakeRect(int x, int y, int width, int height) {
    SDL_Rect rect = {x, y, width, height};
    return rect;
}

SDL_Rect MakeRectFromTo(int x1, int y1, int x2, int y2) {
    SDL_Rect rect = {x1, y1, x2 - x1 + 1, y2 - y1 + 1};
    return rect;
}

void SetRectToMinimumSize(SDL_Rect *rect, int min) {
    if (rect->w < min) {
        rect->x += (rect->w - min) / 2;
        rect->w = min;
    }
    if (rect->h < min) {
        rect->y += (rect->h - min) / 2;
        rect->h = min;
    }
}

void *LoadFile(const char *path, Sint64 *outSize) {
    void *content = NULL;
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    if (!file) {
        printf("LoadFile: %s\n", SDL_GetError());
    } else {
        const Sint64 size = SDL_RWsize(file);
        if (size < 0) {
            printf("LoadFile: %s\n", SDL_GetError());
        } else {
            content = calloc(1, size + 1);
            if (!SDL_RWread(file, content, size, 1)) {
                printf("LoadFile: %s\n", SDL_GetError());
                free(content);
                content = NULL;
            } else {
                if (outSize) {
                    *outSize = size;
                }
            }
        }
        SDL_RWclose(file);
    }
    return content;
}

void GameFilePath(char *dest, const char *subfolder, const char *filename, const char *extension) {
    char *basePath = SDL_GetBasePath();
    if (basePath) {
        strcpy(dest, basePath);
#ifdef SDL2_WORKAROUND
        strcat(dest, "/");
#endif
        SDL_free(basePath);
    } else {
        strcpy(dest, "");
    }
    strcat(dest, "game/");
    if (subfolder) {
        strcat(dest, subfolder);
        strcat(dest, "/");
    }
    strcat(dest, filename);
    if (extension) {
        strcat(dest, ".");
        strcat(dest, extension);
    }
}
