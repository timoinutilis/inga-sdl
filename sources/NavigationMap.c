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

#include "NavigationMap.h"
#include <SDL2/SDL.h>

NavigationMap *LoadNavigationMap(const char *filename) {
    NavigationMap *navigationMap = NULL;
    
    char path[FILENAME_MAX];
    sprintf(path, "game/Dats/%s.ilk", filename);
    
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    if (!file) {
        printf("LoadNavigationMapILK: %s\n", SDL_GetError());
    } else {
        int width = 640;
        int *topLimits = calloc(width, sizeof(int));
        int *bottomLimits = calloc(width, sizeof(int));
        if (topLimits && bottomLimits) {
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
        for (int x = safeX; x != dstX; x += dir) {
            if (navigationMap->topLimits[x] > navigationMap->bottomLimits[x]) {
                destination.x = safeX;
                destination.y = (navigationMap->topLimits[safeX] + navigationMap->bottomLimits[safeX]) * 0.5;
                dstX = safeX;
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
