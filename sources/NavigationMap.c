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

#include "NavigationMap.h"
#include "SDL_includes.h"
#include "Utils.h"

NavigationMap *LoadNavigationMap(const char *filename) {
    NavigationMap *navigationMap = NULL;
    
    char path[FILENAME_MAX];
    GameFilePath(path, "Dats", filename, "ilk");
    
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    if (!file) {
        printf("LoadNavigationMapILK: %s\n", SDL_GetError());
    } else {
        int width = 640;
        int *topLimits = calloc(width, sizeof(int));
        int *bottomLimits = calloc(width, sizeof(int));
        if (!topLimits || !bottomLimits) {
            printf("LoadNavigationMapILK: Out of memory\n");
            free(topLimits);
            free(bottomLimits);
        } else {
            for (int i = 0; i < width; i++) {
                Uint8 value = SDL_ReadU8(file);
                topLimits[i] = value * 2;
            }
            for (int i = 0; i < width; i++) {
                Uint8 value = SDL_ReadU8(file);
                bottomLimits[i] = value * 2;
            }
            navigationMap = calloc(1, sizeof(NavigationMap));
            if (navigationMap) {
                navigationMap->width = width;
                navigationMap->topLimits = topLimits;
                navigationMap->bottomLimits = bottomLimits;
            } else {
                printf("LoadNavigationMapILK: Out of memory\n");
                if (topLimits) free(topLimits);
                if (bottomLimits) free(bottomLimits);
            }
        }
        SDL_RWclose(file);
    }
    return navigationMap;
}

void FreeNavigationMap(NavigationMap *navigationMap) {
    if (!navigationMap) return;
    free(navigationMap->topLimits);
    free(navigationMap->bottomLimits);
    free(navigationMap);
}

NavigationPath *CreateNavigationPath(NavigationMap *navigationMap, Vector origin, Vector destination) {
    NavigationPath *path = calloc(1, sizeof(NavigationPath));
    path->reachesDestination = true;
    
    if (navigationMap) {
        // adjust destination
        AdjustPositionForNavigation(navigationMap, &destination);
                
        // check for end of way
        int dir = (destination.x > origin.x) ? 1 : -1;
        int safeX = fmin(fmax(0, origin.x), navigationMap->width - 1);
        int dstX = fmin(fmax(0, destination.x), navigationMap->width - 1);
        int x = safeX;
        while (x != dstX) {
            x += dir;
            if (navigationMap->topLimits[x] >= navigationMap->bottomLimits[x]) {
                destination.x = safeX;
                destination.y = (navigationMap->topLimits[safeX] + navigationMap->bottomLimits[safeX]) * 0.5;
                path->reachesDestination = false;
                break;
            }
            safeX = x;
        }
        
        // path finding not implemented, relying on AdjustPositionForNavigation
        
        path->numPositions = 1;
        path->positions[0] = destination;
    } else {
        path->numPositions = 1;
        path->positions[0] = destination;
    }
    return path;
}

void FreeNavigationPath(NavigationPath *navigationPath) {
    if (!navigationPath) return;
    free(navigationPath);
}

void AdjustPositionForNavigation(NavigationMap *navigationMap, Vector *position) {
    if (!navigationMap) return;
    int x = fmin(fmax(0, position->x), navigationMap->width - 1);
    int top = navigationMap->topLimits[x];
    int bottom = navigationMap->bottomLimits[x];
    if (position->y < top) {
        position->y = top;
    } else if (position->y > bottom) {
        position->y = bottom;
    }
}

void NavigationMapDrawLine(NavigationMap *navigationMap, int p, int x1, int y1, int x2, int y2) {
    if (!navigationMap) return;
    
    if (x1 > x2) {
        int tw = x1;
        x1 = x2;
        x2 = tw;
        tw = y1;
        y1 = y2;
        y2 = tw;
    }
    
    int br = x2 - x1;
    if (br != 0) {
        float dif = (y2 - y1) / (float)br;
        for (int z = 0; z <= br; z++) {
            if (p == 0) {
                navigationMap->topLimits[x1 + z] = (int)(y1 + (dif * z));
            } else {
                navigationMap->bottomLimits[x1 + z] = (int)(y1 + (dif * z));
            }
        }
    }
}
