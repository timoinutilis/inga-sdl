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

#ifndef Utils_h
#define Utils_h

#include <stdio.h>
#include <SDL2/SDL.h>

typedef struct Vector {
    float x;
    float y;
} Vector;

Vector MakeVector(float x, float y);
SDL_Rect MakeRect(int x, int y, int width, int height);
SDL_Rect MakeRectFromTo(int x1, int y1, int x2, int y2);
void *LoadFile(const char *path);

#endif /* Utils_h */
