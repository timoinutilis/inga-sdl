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

#ifndef Location_h
#define Location_h

#include <stdio.h>
#include <SDL2/SDL.h>
#include "Image.h"
#include "NavigationMap.h"
#include "Element.h"

typedef struct Location {
    int id;
    Image *image;
    Image *foregroundImage;
    NavigationMap *navigationMap;
    Element *rootElement;
} Location;

Location *CreateLocation(int id, const char *background, SDL_Renderer *renderer);
void FreeLocation(Location *location);
void UpdateLocation(Location *location, int deltaTicks);
void DrawLocation(Location *location, SDL_Renderer *renderer);

void AddElement(Location *location, Element *element);
Element *GetElement(Location *location, int id);
Element *GetElementAt(Location *location, int x, int y);

#endif /* Location_h */
