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

#include "Location.h"
#include "Global.h"

void FreeElements(Location *location);
void UpdateElements(Location *location, int deltaTicks);
void DrawElements(Location *location);
void DrawElementOverlays(Location *location);
void SortElements(Location *location);

Location *CreateLocation(int id, const char *background) {
    Location *location = calloc(1, sizeof(Location));
    if (!location) {
        printf("CreateLocation: Out of memory\n");
    } else {
        location->id = id;
        location->image = LoadImage(background, NULL, false, true);
        location->foregroundImage = LoadMaskedImage(background, location->image);
        location->navigationMap = LoadNavigationMap(background);
    }
    return location;
}

void FreeLocation(Location *location) {
    if (!location) return;
    FreeImage(location->image);
    FreeImage(location->foregroundImage);
    FreeNavigationMap(location->navigationMap);
    FreeElements(location);
    free(location);
}

void UpdateLocation(Location *location, int deltaTicks) {
    if (!location) return;
    UpdateElements(location, deltaTicks);
}

void DrawLocation(Location *location) {
    if (!location) return;
    DrawImage(location->image, MakeVector(0, 0));
    DrawElements(location);
    DrawImage(location->foregroundImage, MakeVector(0, 0));
    DrawElementOverlays(location);
}

void AddElement(Location *location, Element *element) {
    if (!location || !element) return;
    element->location = location;
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
        Element *next = element->next;
        if (element->id != MainPersonID) {
            FreeElement(element);
        }
        element = next;
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
    SortElements(location);
}

void DrawElements(Location *location) {
    if (!location) return;
    Element *element = location->rootElement;
    while (element) {
        DrawElement(element);
        element = element->next;
    }
}

void DrawElementOverlays(Location *location) {
    if (!location) return;
    Element *element = location->rootElement;
    while (element) {
        DrawElementOverlay(element);
        element = element->next;
    }
}

void UpdateElementVisibilities(Location *location, GameState *gameState) {
    if (!location || !gameState) return;
    Element *element = location->rootElement;
    while (element) {
        if (element->id != MainPersonID) {
            element->isVisible = GetVisibility(gameState, location->id, element->id);
        }
        element = element->next;
    }
}

void SortElements(Location *location) {
    if (!location) return;
    Element *previous = NULL;
    Element *current = location->rootElement;
    Element *buf1 = NULL;
    Element *buf2 = NULL;
    while (current && current->next) {
        if (current->position.y > current->next->position.y) {
            buf1 = current->next;
            current->next = current->next->next;
            if (previous) {
                buf2 = previous->next;
                previous->next = buf1;
            } else {
                buf2 = location->rootElement;
                location->rootElement = buf1;
            }
            buf1->next = buf2;
            previous = NULL;
            current = location->rootElement;
        } else {
            previous = current;
            current = current->next;
        }
    }
}