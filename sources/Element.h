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

#ifndef Element_h
#define Element_h

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "Config.h"
#include "Image.h"

struct Element {
    int id;
    float x;
    float y;
    SDL_Rect selectionRect;
    int targetX;
    int targetY;
    bool isVisible;
    char name[ELEMENT_NAME_SIZE];
    struct Image *image;
    int frameIndex;
    int frameTicks;
    struct Element *next;
};

struct Element *CreateElement(int id);
void FreeElement(struct Element *element);
void UpdateElement(struct Element *element, int deltaTicks);
void DrawElement(struct Element *element, SDL_Renderer *renderer);

#endif /* Element_h */
