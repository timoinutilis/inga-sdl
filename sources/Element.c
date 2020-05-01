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
#include "Location.h"

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
    FreeNavigationPath(element->navigationPath);
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

void DrawElement(Element *element) {
    if (!element || !element->isVisible) return;
    
    if (element->image && element->image->animation) {
        DrawAnimationFrame(element->image, element->position, element->frameIndex);
    } else {
        DrawImage(element->image, element->position);
    }
}

bool IsPointInElement(Element *element, int x, int y) {
    if (!element || !element->isVisible || !element->isSelectable) return false;
    SDL_Rect rect = element->selectionRect;
    if (rect.w == 0 && element->image) {
        rect.x = element->position.x;
        rect.y = element->position.y;
        if (element->image->animation) {
            Frame *frame = &element->image->animation->frames[element->frameIndex];
            rect.w = frame->sourceRect.w;
            rect.h = frame->sourceRect.h;
            rect.x -= frame->pivot.x;
            rect.y -= frame->pivot.y;
        } else {
            rect.w = element->image->width;
            rect.h = element->image->height;
        }
    }
    if (rect.w > 0) {
        return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
    }
    return false;
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
    FreeNavigationPath(element->navigationPath);
    element->navigationIndex = 0;
    element->navigationPath = CreateNavigationPath(element->location->navigationMap, element->position, MakeVector(x, y));
    if (element->navigationPath) {
        element->action = ElementActionMove;
        Vector firstPosition = element->navigationPath->positions[0];
        ElementLookTo(element, firstPosition.x, firstPosition.y, imageId);
    }
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
    Vector currentPathPosition = element->navigationPath->positions[element->navigationIndex];
    float vx = (currentPathPosition.x - element->position.x);
    float vy = (currentPathPosition.y - element->position.y);
    float nv = sqrt((vx * vx) + (vy * vy));
    float deltaPixels = deltaTicks * element->speed / 1000.0;
    if (nv > deltaPixels) {
        nv = nv / deltaPixels;
    } else {
        if (nv > 0) nv = 1;
    }

    if (nv >= 1) {
        element->position.x += (vx / nv);
        element->position.y += (vy / nv);
        AdjustPositionForNavigation(element->location->navigationMap, &element->position);
//        }
    } else if (element->navigationIndex + 1 < element->navigationPath->numPositions) {
        element->navigationIndex += 1;
        Vector position = element->navigationPath->positions[element->navigationIndex];
        ElementLookTo(element, position.x, position.y, 0);
    } else {
        ElementStop(element);
        if (!element->navigationPath->reachesDestination) {
//            if ((benutzt + angesehen > 0) && (akt->p4 == 2) && (akt->id == 0)) {
//                erreicht = TRUE;
//            } else {
//                benutzt = 0; invbenutzt = 0; angesehen = 0;
//            }
        }
    }

}
