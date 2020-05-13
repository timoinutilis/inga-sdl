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

#ifndef Location_h
#define Location_h

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
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
    Image *foregroundImage;
    NavigationMap *navigationMap;
    Element *rootElement;
    Thread *rootThread;
} Location;

Location *CreateLocation(int id, const char *background);
void FreeLocation(Location *location);
void UpdateLocation(Location *location, int deltaTicks);
void DrawLocation(Location *location);

void LoadLocationBackground(Location *location, const char *background);

void AddElement(Location *location, Element *element);
Element *GetElement(Location *location, int id);
Element *GetElementAt(Location *location, int x, int y);

void UpdateElementVisibilities(Location *location, GameState *gameState);

void AddThread(Location *location, Thread *thread);
Thread *GetThread(Location *location, int id);

#endif /* Location_h */
