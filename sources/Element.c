//
// Copyright (c) 2020 Timo Kloss
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include "Element.h"
#include "Location.h"
#include "Global.h"
#include "Game.h"

const int MainPersonID = 0;
const int ForegroundID = 0xFFFF;

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
            int numFrames = element->image->animation->numFrames;
            if (element->frameIndex + 1 < numFrames) {
                element->frameIndex += 1;
            } else {
                if (element->loopCount > 0) {
                    element->loopCount -= 1;
                    if (element->loopCount > 0) {
                        element->frameIndex = 0;
                    }
                } else {
                    element->frameIndex = 0;
                }
            }
        }
    }
    
    // Action
    switch (element->action) {
        case ElementActionIdle:
            if (element->imageId && element->imageId != element->defaultIdleImageId) {
                SetElementImageFromSet(element, element->defaultIdleImageId);
            }
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
            if ((element->frameIndex == element->takeFrame || element->loopCount == 0) && element->takeElement) {
                element->takeElement->isVisible = false;
            }
            if (element->loopCount == 0) {
                ElementStop(element);
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
#ifdef TOUCH
        SetRectToMinimumSize(&rect, 33);
#endif
        return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
    }
    return false;
}

void ResetDefaultAnimations(Element *element) {
    SetDefaultIdleImageId(element, 1);
    SetDefaultWalkImageId(element, 2);
}

void SetDefaultIdleImageId(Element *element, int imageId) {
    if (!element) return;
    element->defaultIdleImageId = imageId;
    if (element->action == ElementActionIdle) {
        SetElementImageFromSet(element, imageId);
    }
}

void SetDefaultWalkImageId(Element *element, int imageId) {
    if (!element) return;
    element->defaultWalkImageId = imageId;
    if (element->action == ElementActionMove) {
        SetElementImageFromSet(element, imageId);
    }
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
    Image *image = GetImageFromSet(element->imageSet, imageId, element->direction);
    if (!image) {
        printf("SetElementImageFromSet: Image ID %d not found\n", imageId);
        return;
    }
    element->imageId = imageId;
    element->image = image;
    element->frameIndex = 0;
    element->frameTicks = 0;
}

void ElementFreeAction(Element *element) {
    if (!element) return;
    
    FreeImage(element->talkImage);
    element->talkImage = NULL;
    
    FreeNavigationPath(element->navigationPath);
    element->navigationPath = NULL;
    
    element->takeElement = NULL;
}

void ElementStop(Element *element) {
    if (!element) return;
    ElementFreeAction(element);
    element->action = ElementActionIdle;
    // resets animation in update
}

void ElementSetDirection(Element *element, Vector direction) {
    if (!element) return;
    element->direction = direction;
    SetElementImageFromSet(element, element->imageId);
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

void ElementTalk(Element *element, const char *text, int imageId, Font *font, int textSpeed) {
    if (!element) return;
    ElementFreeAction(element);
    element->action = ElementActionTalk;
    SetElementImageFromSet(element, imageId);
    Frame *frame = &element->image->animation->frames[element->frameIndex];
    SDL_Color color = {255, 255, 255, 255};
    element->talkImage = CreateImageFromText(text, font, color);
    if (element->talkImage) {
        if (element->position.y - frame->pivot.y > 0) {
            // top of head
            element->talkOffset = MakeVector(element->talkImage->width * -0.5, -frame->pivot.y - 32);
        } else {
            // below feet
            element->talkOffset = MakeVector(element->talkImage->width * -0.5, frame->sourceRect.h - frame->pivot.y);
        }
    }
    element->talkTicks = (int)strlen(text) * 30 * (textSpeed + 1) + 1000;
}

void ElementSkipTalk(Element *element) {
    if (!element || element->action != ElementActionTalk) return;
    element->talkTicks = 0;
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
