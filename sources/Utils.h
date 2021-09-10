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

#ifndef Utils_h
#define Utils_h

#include <stdio.h>
#include "SDL_includes.h"

typedef struct Vector {
    float x;
    float y;
} Vector;

Vector MakeVector(float x, float y);
SDL_Rect MakeRect(int x, int y, int width, int height);
SDL_Rect MakeRectFromTo(int x1, int y1, int x2, int y2);
void SetRectToMinimumSize(SDL_Rect *rect, int min);
void *LoadFile(const char *path, Sint64 *outSize);
void GameFilePath(char *dest, const char *subfolder, const char *filename, const char *extension);

#endif /* Utils_h */
