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

#include "ImageSet.h"

struct ImageSet *LoadImageSetIPE(const char *filename, SDL_Renderer *renderer, SDL_Palette *defaultPalette, bool createMasks) {
    struct ImageSet *imageSet = NULL;
    
    char path[FILENAME_MAX];
    sprintf(path, "game/Dats/%s.ipe", filename);
    
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    if (!file) {
        printf("LoadImageSetIPE: %s\n", SDL_GetError());
    } else {
        Uint16 numItems = SDL_ReadBE16(file);
        
        struct ImageSetItem *items = calloc(numItems, sizeof(struct ImageSetItem));
        if (items) {
            char itemFilename[31];
            for (int i = 0; i < numItems; i++) {
                struct ImageSetItem *item = &items[i];
                
                item->id = SDL_ReadBE16(file);
                item->side = SDL_ReadU8(file);
                SDL_RWread(file, itemFilename, sizeof(char), 31);
                strncpy(item->filename, itemFilename, FILE_NAME_SIZE);
            }
            imageSet = calloc(1, sizeof(struct ImageSet));
            if (imageSet) {
                imageSet->numItems = numItems;
                imageSet->items = items;
                imageSet->renderer = renderer;
                imageSet->defaultPalette = defaultPalette;
                imageSet->createsMasks = createMasks;
            } else {
                free(items);
            }
        }
    }
    return imageSet;

}

void FreeImageSet(struct ImageSet *imageSet) {
    if (!imageSet) return;
    for (int i = 0; i < imageSet->numItems; i++) {
        struct ImageSetItem *item = &imageSet->items[i];
        if (item->image) {
            FreeImage(item->image);
        }
    }
    free(imageSet->items);
    free(imageSet);
}

struct Image *GetImageFromSet(struct ImageSet *imageSet, int id, enum ImageSide side) {
    if (!imageSet) return NULL;
    struct ImageSetItem *sideItems[4] = {NULL, NULL, NULL, NULL};
    for (int i = 0; i < imageSet->numItems; i++) {
        struct ImageSetItem *item = &imageSet->items[i];
        if (item->id == id) {
            sideItems[side] = item;
            if (item->side == side) {
                break;
            }
        }
    }
    
    struct ImageSetItem *bestItem = sideItems[side];
    if (!bestItem) {
        bestItem = sideItems[ImageSideFront];
    }
    if (!bestItem) {
        bestItem = sideItems[ImageSideLeft];
    }
    if (!bestItem) {
        bestItem = sideItems[ImageSideRight];
    }
    if (!bestItem) {
        bestItem = sideItems[ImageSideBack];
    }
    if (bestItem) {
        if (!bestItem->image) {
            bestItem->image = LoadImageIBM(bestItem->filename, imageSet->renderer, imageSet->defaultPalette, imageSet->createsMasks);
        }
        return bestItem->image;
    }
    return NULL;
}
