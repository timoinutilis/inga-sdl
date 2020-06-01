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

#include "Element.h"
#include "Location.h"
#include "Global.h"
#include "Game.h"

void SetElementImageFromSet(Element *element, int imageId);
void ElementFreeAction(Element *element);
void UpdateMove(Element *element, int deltaTicks);

Element *CreateElement(int id) {
    Element *element = calloc(1, sizeof(struct Element));
    if (!element) {
        printf("CreateElement: Out of memory\n");
    } else {
        element->id = id;
        element->isVisible = true;
        element->speed = 240;
        ResetDefaultAnimations(element);
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
    ElementFreeAction(element);
    free(element);
}

void UpdateElement(Element *element, int deltaTicks) {
    if (!element || !element->isVisible) return;
    
    if (!element->image && element->imageSet) {
        SetElementImageFromSet(element, element->defaultIdleImageId);
    }
    
    // Animation
    if (!element->isPaused && element->image && element->image->animation) {
        element->frameTicks += deltaTicks;
        if (element->frameTicks >= element->image->animation->frames[element->frameIndex].ticks) {
            element->frameTicks = 0;
            element->frameIndex += 1;
            int numFrames = element->image->animation->numFrames;
            if (element->frameIndex >= numFrames) {
                element->frameIndex = 0;
                if (element->loopCount > 0) {
                    element->loopCount -= 1;
                }
            }
        }
    }
    
    // Action
    switch (element->action) {
        case ElementActionIdle:
            break;
        case ElementActionMove:
            UpdateMove(element, deltaTicks);
            break;
        case ElementActionTalk:
            if (element->talkTicks <= 0) {
                ElementStop(element);
            } else {
                element->talkTicks -= deltaTicks;
            }
            break;
        case ElementActionAnimate:
            if (element->loopCount == 0) {
                ElementStop(element);
            }
            break;
        case ElementActionTake:
            if (element->loopCount == 0) {
                ElementStop(element);
            } else if (element->frameIndex == element->takeFrame && element->takeElement) {
                element->takeElement->isVisible = false;
            }
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

void DrawElementOverlay(Element *element) {
    if (!element || !element->isVisible) return;
    
    if (element->talkImage) {
        Vector position = element->position;
        position.x = fmin(fmax(0, position.x + element->talkOffset.x), SCREEN_WIDTH - element->talkImage->width);
        position.y = fmin(fmax(0, position.y + element->talkOffset.y), SCREEN_HEIGHT - element->talkImage->height);
        DrawImage(element->talkImage, position);
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

void ResetDefaultAnimations(Element *element) {
    if (!element) return;
    element->defaultIdleImageId = 1;
    element->defaultWalkImageId = 2;
}

Vector GetElementTarget(Element *element, Vector fromPosition) {
    if (element->target.y) {
        return element->target;
    } else {
        float dx = fromPosition.x - element->position.x;
        float dy = fromPosition.y - element->position.y;
        if (dx * dx < 1 && dy * dy < 1) {
            dx = 1;
        }
        float dist = sqrtf(dx * dx + dy * dy);
        dx = dx / dist * 100;
        dy = dy / dist * 40;
        
        Vector target = element->position;
        target.x += dx;
        target.y += dy;
        return target;
    }
}

void SetElementImageFromSet(Element *element, int imageId) {
    if (!element || !element->imageSet) return;
    element->imageId = imageId;
    element->image = GetImageFromSet(element->imageSet, imageId, element->direction);
    element->frameIndex = 0;
    element->frameTicks = 0;
}

void ElementFreeAction(Element *element) {
    if (!element) return;
    
    FreeImage(element->talkImage);
    element->talkImage = NULL;
    
    FreeNavigationPath(element->navigationPath);
    element->navigationPath = NULL;
}

void ElementStop(Element *element) {
    if (!element) return;
    ElementFreeAction(element);
    element->action = ElementActionIdle;
    SetElementImageFromSet(element, element->defaultIdleImageId);
}

void ElementSetSide(Element *element, ImageSide side, int imageId) {
    if (!element) return;
    switch (side) {
        case ImageSideFront:
            element->direction.x = 0;
            element->direction.y = 1;
            break;
        case ImageSideBack:
            element->direction.x = 0;
            element->direction.y = -1;
            break;
        case ImageSideLeft:
            element->direction.x = -1;
            element->direction.y = 0;
            break;
        case ImageSideRight:
            element->direction.x = 1;
            element->direction.y = 0;
            break;
    }
    SetElementImageFromSet(element, imageId ? imageId : element->imageId);
}

void ElementLookTo(Element *element, int x, int y, int imageId) {
    if (!element) return;
    if (x != element->position.x || y != element->position.y) {
        element->direction.x = (x - element->position.x);
        element->direction.y = (y - element->position.y);
    }
    SetElementImageFromSet(element, imageId ? imageId : element->imageId);
}

void ElementMoveTo(Element *element, int x, int y, int imageId, bool ignoreNavMap) {
    if (!element) return;
    ElementFreeAction(element);
    element->navigationIndex = 0;
    element->navigationIgnoresMap = ignoreNavMap;
    element->navigationPath = CreateNavigationPath(ignoreNavMap ? NULL : element->location->navigationMap, element->position, MakeVector(x, y));
    if (element->navigationPath) {
        element->action = ElementActionMove;
        Vector firstPosition = element->navigationPath->positions[0];
        ElementLookTo(element, firstPosition.x, firstPosition.y, imageId ? imageId : element->defaultWalkImageId);
    }
}

void ElementTalk(Element *element, const char *text, int imageId, Font *font) {
    if (!element) return;
    ElementFreeAction(element);
    element->action = ElementActionTalk;
    SetElementImageFromSet(element, imageId);
    Frame *frame = &element->image->animation->frames[element->frameIndex];
    SDL_Color color = {255, 255, 255, 255};
    element->talkImage = CreateImageFromText(text, font, color);
    element->talkOffset = element->talkImage ? MakeVector(element->talkImage->width * -0.5, -frame->pivot.y - 24) : MakeVector(0, 0);
    element->talkTicks = (int)strlen(text) * 25 + 1000;
}

void ElementAnimate(Element *element, int imageId, int loopCount) {
    if (!element) return;
    ElementFreeAction(element);
    element->action = ElementActionAnimate;
    element->loopCount = loopCount;
    SetElementImageFromSet(element, imageId);
}

void ElementTake(Element *element, int imageId, Element *takeElement, int takeFrame) {
    if (!element || !takeElement) return;
    ElementFreeAction(element);
    element->action = ElementActionTake;
    element->loopCount = 1;
    element->takeElement = takeElement;
    element->takeFrame = takeFrame;
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
        if (!element->navigationIgnoresMap) {
            AdjustPositionForNavigation(element->location->navigationMap, &element->position);
        }
    } else if (element->navigationIndex + 1 < element->navigationPath->numPositions) {
        element->navigationIndex += 1;
        Vector position = element->navigationPath->positions[element->navigationIndex];
        ElementLookTo(element, position.x, position.y, 0);
    } else {
//        bool reached = element->navigationPath->reachesDestination;
        ElementStop(element);
        if (element->id == MainPersonID) {
            MainPersonDidFinishWalking(element->location->game);
        }
//        if (reached) {
//        } else {
//            if ((benutzt + angesehen > 0) && (akt->p4 == 2) && (akt->id == 0)) {
//                erreicht = TRUE;
//            } else {
//                benutzt = 0; invbenutzt = 0; angesehen = 0;
//            }
//        }
    }

}
