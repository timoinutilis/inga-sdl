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
#include "Element.h"

struct Location {
    int id;
    struct Image *image;
    struct Element *rootElement;
};

struct Location *CreateLocation(int id, const char *background, SDL_Renderer *renderer);
void FreeLocation(struct Location *location);
void UpdateLocation(struct Location *location, int deltaTicks);
void DrawLocation(struct Location *location, SDL_Renderer *renderer);

void AddElement(struct Location *location, struct Element *element);

#endif /* Location_h */
