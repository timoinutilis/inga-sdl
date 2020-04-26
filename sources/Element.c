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

void SetElementImageFromSet(Element *element, int imageId);
void UpdateMove(Element *element, int deltaTicks);

Element *CreateElement(int id) {
    Element *element = calloc(1, sizeof(struct Element));
    if (!element) {
        printf("CreateElement: Out of memory\n");
    } else {
        element->id = id;
        element->isVisible = true;
        element->speed = 300;
    }
    return element;
}

void FreeElement(Element *element) {
    if (!element) return;
    if (element->imageSet) {
        FreeImageSet(element->imageSet);
    } else {
        FreeImage(element->image);
    }
    free(element);
}

void UpdateElement(Element *element, int deltaTicks) {
    if (!element || !element->isVisible) return;
    
    if (!element->image && element->imageSet) {
        SetElementImageFromSet(element, 1);
    }
    
    if (element->image && element->image->animation) {
        element->frameTicks += deltaTicks;
        if (element->frameTicks >= element->image->animation->frames[element->frameIndex].ticks) {
            element->frameTicks = 0;
            element->frameIndex = (element->frameIndex + 1) % element->image->animation->numFrames;
        }
    }
    
    switch (element->action) {
        case ElementActionIdle:
            break;
        case ElementActionMove:
            UpdateMove(element, deltaTicks);
            break;
        case ElementActionTalk:
            break;
        case ElementActionAnimate:
            break;
    }
}

void DrawElement(Element *element, SDL_Renderer *renderer) {
    if (!element || !element->isVisible) return;
    
    if (element->image && element->image->animation) {
        DrawAnimationFrame(element->image, renderer, element->position, element->frameIndex);
    } else {
        DrawImage(element->image, renderer, element->position);
    }
}

bool IsPointInElement(Element *element, int x, int y) {
    if (!element || !element->isVisible || element->selectionRect.w == 0) return false;
    return true;
}

void SetElementImageFromSet(Element *element, int imageId) {
    if (!element || !element->imageSet) return;
    element->imageId = imageId;
    element->image = GetImageFromSet(element->imageSet, imageId, element->direction);
    element->frameIndex = 0;
    element->frameTicks = 0;
}

void ElementStop(Element *element) {
    if (!element) return;
    element->action = ElementActionIdle;
    SetElementImageFromSet(element, 1);
}

void ElementLookTo(Element *element, int x, int y, int imageId) {
    if (!element) return;
    element->direction.x = (x - element->position.x);
    element->direction.y = (y - element->position.y);
    SetElementImageFromSet(element, imageId ? imageId : element->imageId);
}

void ElementMoveTo(Element *element, int x, int y, int imageId) {
    if (!element) return;
    element->action = ElementActionMove;
    element->movingTo = MakeVector(x, y);
    ElementLookTo(element, x, y, imageId);
}

void ElementTalk(Element *element, const char *text, int imageId) {
    if (!element) return;
    element->action = ElementActionTalk;
    SetElementImageFromSet(element, imageId);
}

void ElementAnimate(Element *element, int imageId) {
    if (!element) return;
    element->action = ElementActionAnimate;
    SetElementImageFromSet(element, imageId);
}

void UpdateMove(Element *element, int deltaTicks) {
    float vx = (element->movingTo.x - element->position.x);
    float vy = (element->movingTo.y - element->position.y);
    float nv = sqrt((vx * vx) + (vy * vy));
    float deltaPixels = deltaTicks * element->speed / 1000.0;
    if (nv > deltaPixels) {
        nv = nv / deltaPixels;
    } else {
        if (nv > 0) nv = 1;
    }

    if (nv >= 1) {
        float tp = element->position.x + (vx / nv);
//        if ((ilk.oben[(WORD)tp] > ilk.unten[(WORD)tp]) && (akt->p4 != 1)) {
//            akt->aktion = AKT_NICHTS;
//            if ((benutzt + angesehen > 0) && (akt->p4 == 2) && (akt->id == 0)) {
//                erreicht = TRUE;
//            } else {
//                benutzt = 0; invbenutzt = 0; angesehen = 0;
//            }
//        } else {
        element->position.x = tp;
        element->position.y += (vy / nv);
//        if (akt->p4 != 1) {
//            if ((akt->y) > (ilk.unten[(WORD)akt->x] * 2)) akt->y = ilk.unten[(WORD)akt->x] * 2;
//            if ((akt->y) < (ilk.oben[(WORD)akt->x] * 2)) akt->y = ilk.oben[(WORD)akt->x] * 2;
//        }
//        }
    } else {
        ElementStop(element);
    }

}
