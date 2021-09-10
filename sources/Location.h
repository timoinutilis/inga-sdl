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

#ifndef Location_h
#define Location_h

#include <stdio.h>
#include <stdbool.h>
#include "SDL_includes.h"
#include "Image.h"
#include "NavigationMap.h"
#include "Element.h"
#include "GameState.h"
#include "Thread.h"

typedef struct Game Game;

typedef struct Location {
    int id;
    Game *game;
    Image *image;
    NavigationMap *navigationMap;
    Element *rootElement;
    Thread *rootThread;
} Location;

Location *CreateLocation(int id, const char *background);
void FreeLocation(Location *location);
void UpdateLocation(Location *location, int deltaTicks);
void DrawLocation(Location *location);
void DrawLocationOverlays(Location *location);

void LoadLocationBackground(Location *location, const char *background);
void LoadLocationNavigationMap(Location *location, const char *background);

void AddElement(Location *location, Element *element);
Element *GetElement(Location *location, int id);
Element *GetElementAt(Location *location, int x, int y);

void UpdateElementVisibilities(Location *location, GameState *gameState);

void AddThread(Location *location, Thread *thread);
Thread *GetThread(Location *location, int id);

#endif /* Location_h */
