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

#ifndef ImageSet_h
#define ImageSet_h

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "Config.h"
#include "Image.h"
#include "Utils.h"

typedef enum ImageSide {
    ImageSideLeft,
    ImageSideRight,
    ImageSideFront,
    ImageSideBack
} ImageSide;

typedef struct ImageSetItem {
    int id;
    ImageSide side;
    char filename[FILE_NAME_SIZE];
    struct Image *image;
} ImageSetItem;

typedef struct ImageSet {
    int numItems;
    struct ImageSetItem *items;
    SDL_Palette *defaultPalette;
    bool createsMasks;
} ImageSet;

ImageSet *LoadImageSet(const char *filename, SDL_Palette *defaultPalette, bool createMasks);
void FreeImageSet(ImageSet *imageSet);
Image *GetImageFromSet(ImageSet *imageSet, int id, Vector direction);

#endif /* ImageSet_h */
