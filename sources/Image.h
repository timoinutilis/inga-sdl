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

#ifndef Image_h
#define Image_h

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "Utils.h"

typedef struct Frame {
    SDL_Rect sourceRect;
    int pivotX;
    int pivotY;
    int ticks;
} Frame;

typedef struct Animation {
    int numFrames;
    Frame *frames;
} Animation;

typedef struct Image {
    SDL_Texture *texture;
    SDL_Palette *palette;
    int width;
    int height;
    Animation *animation;
} Image;

enum StripDirection {
    StripDirectionHorizontal,
    StripDirectionVertical
};

Image *LoadImageIBM(const char *filename, SDL_Renderer *renderer, SDL_Palette *defaultPalette, bool createMask);
void FreeImage(Image *image);
void DrawImage(Image *image, SDL_Renderer *renderer, Vector position);
void DrawAnimationFrame(Image *image, SDL_Renderer *renderer, Vector position, int index);

#endif /* Image_h */
