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
void FreeThreads(Location *location);
void UpdateThreads(Location *location);

Location *CreateLocation(int id, const char *background) {
    Location *location = calloc(1, sizeof(Location));
    if (!location) {
        printf("CreateLocation: Out of memory\n");
    } else {
        location->id = id;
        LoadLocationBackground(location, background);
        LoadLocationNavigationMap(location, background);
    }
    return location;
}

void FreeLocation(Location *location) {
    if (!location) return;
    FreeImage(location->image);
    FreeNavigationMap(location->navigationMap);
    FreeElements(location);
    FreeThreads(location);
    free(location);
}

void UpdateLocation(Location *location, int deltaTicks) {
    if (!location) return;
    UpdateThreads(location);
    UpdateElements(location, deltaTicks);
}

void DrawLocation(Location *location) {
    if (!location) return;
    DrawImage(location->image, MakeVector(0, 0));
    DrawElements(location);
    DrawElementOverlays(location);
}

void LoadLocationBackground(Location *location, const char *background) {
    if (!location) return;
    Image *prevImage = location->image;
    location->image = LoadImage(background, prevImage ? prevImage->surface->format->palette : NULL, false, true);
    FreeImage(prevImage);
    
    Element *foreground = GetElement(location, ForegroundID);
    if (foreground) {
        FreeImage(foreground->image);
    } else {
        foreground = CreateElement(ForegroundID);
        foreground->layer = LayerForeground;
        AddElement(location, foreground);
    }
    foreground->image = LoadMaskedImage(background, location->image);
}

void LoadLocationNavigationMap(Location *location, const char *background) {
    if (!location) return;
    if (location->navigationMap) {
        FreeNavigationMap(location->navigationMap);
    }
    location->navigationMap = LoadNavigationMap(background);
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
    Element *elementAtPoint = NULL;
    while (element) {
        if (IsPointInElement(element, x, y) && (!elementAtPoint || element->id > elementAtPoint->id)) {
            elementAtPoint = element;
        }
        element = element->next;
    }
    return elementAtPoint;
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
        if (   current->layer > current->next->layer
            || (current->layer == LayerPersons && current->layer == current->next->layer && current->position.y > current->next->position.y)) {
            
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

void AddThread(Location *location, Thread *thread) {
    if (!location || !thread) return;
    thread->next = location->rootThread;
    location->rootThread = thread;
}

Thread *GetThread(Location *location, int id) {
    if (!location) return NULL;
    Thread *thread = location->rootThread;
    while (thread) {
        if (thread->id == id) {
            return thread;
        }
        thread = thread->next;
    }
    return NULL;
}

void FreeThreads(Location *location) {
    if (!location) return;
    Thread *thread = location->rootThread;
    while (thread) {
        Thread *next = thread->next;
        FreeThread(thread);
        thread = next;
    }
    location->rootThread = NULL;
}

void UpdateThreads(Location *location) {
    if (!location) return;
    Thread *thread = location->rootThread;
    while (thread) {
        UpdateThread(thread, location->game);
        thread = thread->next;
    }
}
