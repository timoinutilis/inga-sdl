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

ImageSet *LoadImageSet(const char *filename, SDL_Palette *defaultPalette, bool createMasks) {
    ImageSet *imageSet = NULL;
    
    char path[FILENAME_MAX];
    sprintf(path, "game/Dats/%s.ipe", filename);
    
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    if (!file) {
        printf("LoadImageSetIPE: %s\n", SDL_GetError());
    } else {
        Uint16 numItems = SDL_ReadBE16(file);
        
        ImageSetItem *items = calloc(numItems, sizeof(ImageSetItem));
        if (items) {
            char itemFilename[31];
            for (int i = 0; i < numItems; i++) {
                ImageSetItem *item = &items[i];
                
                item->id = SDL_ReadBE16(file);
                item->side = SDL_ReadU8(file);
                SDL_RWread(file, itemFilename, sizeof(char), 31);
                strncpy(item->filename, itemFilename, FILE_NAME_SIZE);
            }
            imageSet = calloc(1, sizeof(ImageSet));
            if (imageSet) {
                imageSet->numItems = numItems;
                imageSet->items = items;
                imageSet->defaultPalette = defaultPalette;
                imageSet->createsMasks = createMasks;
            } else {
                free(items);
            }
        }
        SDL_RWclose(file);
    }
    return imageSet;
}

void FreeImageSet(ImageSet *imageSet) {
    if (!imageSet) return;
    for (int i = 0; i < imageSet->numItems; i++) {
        ImageSetItem *item = &imageSet->items[i];
        if (item->image) {
            FreeImage(item->image);
        }
    }
    free(imageSet->items);
    free(imageSet);
}

Image *GetImageFromSet(ImageSet *imageSet, int id, Vector direction) {
    if (!imageSet) return NULL;
    
    ImageSetItem *sideItems[4] = {NULL, NULL, NULL, NULL};
    ImageSetItem *anyItem = NULL;
    for (int i = 0; i < imageSet->numItems; i++) {
        ImageSetItem *item = &imageSet->items[i];
        if (item->id == id) {
            sideItems[item->side] = item;
            anyItem = item;
        }
    }
    
    ImageSetItem *bestItem = NULL;
    
    if (fabs(direction.y) >= fabs(direction.x)) {
        if (direction.y >= 0) {
            bestItem = sideItems[ImageSideFront];
        } else {
            bestItem = sideItems[ImageSideBack];
        }
        if (!bestItem) {
            if (direction.x <= 0) {
                bestItem = sideItems[ImageSideLeft];
            } else {
                bestItem = sideItems[ImageSideRight];
            }
        }
    } else {
        if (direction.x <= 0) {
            bestItem = sideItems[ImageSideLeft];
        } else {
            bestItem = sideItems[ImageSideRight];
        }
        if (!bestItem) {
            if (direction.y >= 0) {
                bestItem = sideItems[ImageSideFront];
            } else {
                bestItem = sideItems[ImageSideBack];
            }
        }
    }
    if (!bestItem) {
        bestItem = anyItem;
    }
    
    if (bestItem) {
        if (!bestItem->image) {
            bestItem->image = LoadImage(bestItem->filename, imageSet->defaultPalette, imageSet->createsMasks, false);
        }
        return bestItem->image;
    }
    return NULL;
}
