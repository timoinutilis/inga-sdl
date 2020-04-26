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

#include "Location.h"

void FreeElements(Location *location);
void UpdateElements(Location *location, int deltaTicks);
void DrawElements(Location *location, SDL_Renderer *renderer);

Location *CreateLocation(int id, const char *background, SDL_Renderer *renderer) {
    Location *location = calloc(1, sizeof(Location));
    if (!location) {
        printf("CreateLocation: Out of memory\n");
    } else {
        location->id = id;
        location->image = LoadImageIBM(background, renderer, NULL, false);
    }
    return location;
}

void FreeLocation(Location *location) {
    if (!location) return;
    FreeImage(location->image);
    FreeElements(location);
    free(location);
}

void UpdateLocation(Location *location, int deltaTicks) {
    if (!location) return;
    UpdateElements(location, deltaTicks);
}

void DrawLocation(Location *location, SDL_Renderer *renderer) {
    if (!location) return;
    DrawImage(location->image, renderer, MakeVector(0, 0));
    DrawElements(location, renderer);
}

void AddElement(Location *location, Element *element) {
    if (!location || !element) return;
    element->next = location->rootElement;
    location->rootElement = element;
}

Element *GetElement(Location *location, int id) {
    if (!location) return NULL;
    Element *element = location->rootElement;
    while (element) {
        if (element->id == id) {
            return element;
        }
        element = element->next;
    }
    return NULL;
}

Element *GetElementAt(Location *location, int x, int y) {
    if (!location) return NULL;
    Element *element = location->rootElement;
    while (element) {
        if (IsPointInElement(element, x, y)) {
            return element;
        }
        element = element->next;
    }
    return NULL;
}

void FreeElements(Location *location) {
    if (!location) return;
    Element *element = location->rootElement;
    while (element) {
        FreeElement(element);
        element = element->next;
    }
    location->rootElement = NULL;
}

void UpdateElements(Location *location, int deltaTicks) {
    if (!location) return;
    Element *element = location->rootElement;
    while (element) {
        UpdateElement(element, deltaTicks);
        element = element->next;
    }
}

void DrawElements(Location *location, SDL_Renderer *renderer) {
    if (!location) return;
    Element *element = location->rootElement;
    while (element) {
        DrawElement(element, renderer);
        element = element->next;
    }
}
