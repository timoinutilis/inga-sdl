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
