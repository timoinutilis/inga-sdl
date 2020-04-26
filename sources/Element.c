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

#include "Element.h"

struct Element *CreateElement(int id) {
    struct Element *element = calloc(1, sizeof(struct Element));
    if (!element) {
        printf("CreateElement: Out of memory\n");
    } else {
        element->id = id;
        element->isVisible = true;
    }
    return element;
}

void FreeElement(struct Element *element) {
    if (!element) return;
    free(element);
}

void UpdateElement(struct Element *element, int deltaTicks) {
    if (!element || !element->isVisible) return;
    
    if (element->image && element->image->animation) {
        element->frameTicks += deltaTicks;
        if (element->frameTicks >= element->image->animation->frames[element->frameIndex].ticks) {
            element->frameTicks = 0;
            element->frameIndex = (element->frameIndex + 1) % element->image->animation->numFrames;
        }
    }
}

void DrawElement(struct Element *element, SDL_Renderer *renderer) {
    if (!element || !element->isVisible) return;
    
    if (element->image && element->image->animation) {
        DrawAnimationFrame(element->image, renderer, element->x, element->y, element->frameIndex);
    } else {
        DrawImage(element->image, renderer, element->x, element->y);
    }
}
