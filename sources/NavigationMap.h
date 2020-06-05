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

#ifndef NavigationMap_h
#define NavigationMap_h

#include <stdio.h>
#include <stdbool.h>
#include "Utils.h"
#include "Config.h"

typedef struct NavigationMap {
    int width;
    int *topLimits;
    int *bottomLimits;
} NavigationMap;

typedef struct NavigationPath {
    int numPositions;
    Vector positions[MAX_PATH_SIZE];
    bool reachesDestination;
} NavigationPath;

NavigationMap *LoadNavigationMap(const char *filename);
void FreeNavigationMap(NavigationMap *navigationMap);
NavigationPath *CreateNavigationPath(NavigationMap *navigationMap, Vector origin, Vector destination);
void FreeNavigationPath(NavigationPath *navigationPath);
void AdjustPositionForNavigation(NavigationMap *navigationMap, Vector *position);

void NavigationMapDrawLine(NavigationMap *navigationMap, int p, int x1, int y1, int x2, int y2);

#endif /* NavigationMap_h */
