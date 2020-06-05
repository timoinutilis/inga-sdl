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

void *LoadFile(const char *path, Sint64 *outSize) {
    void *content = NULL;
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    if (!file) {
        printf("LoadFile: %s\n", SDL_GetError());
    } else {
        Sint64 size = SDL_RWsize(file);
        if (size < 0) {
            printf("LoadFile: %s\n", SDL_GetError());
        } else {
            content = malloc(size);
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
