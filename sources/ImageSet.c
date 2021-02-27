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

#include "ImageSet.h"
#include "Utils.h"

ImageSet *LoadImageSet(const char *filename, SDL_Palette *defaultPalette, bool createMasks) {
    ImageSet *imageSet = NULL;
    
    char path[FILENAME_MAX];
    GameFilePath(path, "Dats", filename, "ipe");
    
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

Vector DirectionForSide(const ImageSide side) {
    switch (side) {
        case ImageSideLeft:
            return MakeVector(-1, 0);
        case ImageSideRight:
            return MakeVector(1, 0);
        case ImageSideFront:
            return MakeVector(0, 1);
        case ImageSideBack:
            return MakeVector(0, -1);
    }
    return MakeVector(0,0);
}
