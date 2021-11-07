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

#ifndef Element_h
#define Element_h

#include <stdio.h>
#include <stdbool.h>
#include "SDL_includes.h"
#include "Config.h"
#include "Utils.h"
#include "Image.h"
#include "ImageSet.h"
#include "NavigationMap.h"
#include "Font.h"
#include "Enums.h"

typedef struct Location Location;

extern const int MainPersonID;
extern const int ForegroundID;

typedef enum ElementAction {
    ElementActionIdle,
    ElementActionMove,
    ElementActionTalk,
    ElementActionAnimate,
    ElementActionTake
} ElementAction;

typedef enum Layer {
    LayerDeco,
    LayerObjects,
    LayerFields,
    LayerPersons,
    LayerForeground,
    LayerTop
} Layer;

typedef struct Element {
    int id;
    Location *location;
    Vector position;
    Layer layer;
    bool isSelectable;
    SDL_Rect selectionRect;
    Vector target;
    bool isVisible;
    char name[ELEMENT_NAME_SIZE];
    Image *image;
    int frameIndex;
    int frameTicks;
    int loopCount;
    bool isPaused;
    ImageSet *imageSet;
    int imageId;
    Vector direction;
    ElementAction action;
    NavigationPath *navigationPath;
    bool navigationIgnoresMap;
    int navigationIndex;
    float speed;
    Image *talkImage;
    Vector talkOffset;
    int talkTicks;
    int defaultIdleImageId;
    int defaultWalkImageId;
    struct Element *takeElement;
    int takeFrame;
    struct Element *next;
} Element;

Element *CreateElement(int id);
void FreeElement(Element *element);
void UpdateElement(Element *element, int deltaTicks);
void DrawElement(Element *element);
void DrawElementOverlay(Element *element);
bool IsPointInElement(Element *element, int x, int y);
void ResetDefaultAnimations(Element *element);
void SetDefaultIdleImageId(Element *element, int imageId);
void SetDefaultWalkImageId(Element *element, int imageId);
Vector GetElementTarget(Element *element, Vector fromPosition);

void ElementStop(Element *element);
void ElementSetDirection(Element *element, Vector direction);
void ElementLookTo(Element *element, int x, int y, int imageId);
void ElementMoveTo(Element *element, int x, int y, int imageId, bool ignoreNavMap);
void ElementTalk(Element *element, const char *text, int imageId, Font *font, int textSpeed);
void ElementSkipTalk(Element *element);
void ElementAnimate(Element *element, int imageId, int loopCount);
void ElementTake(Element *element, int imageId, Element *takeElement, int takeFrame);

#endif /* Element_h */
