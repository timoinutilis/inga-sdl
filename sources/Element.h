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

#ifndef Element_h
#define Element_h

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "Config.h"
#include "Utils.h"
#include "Image.h"
#include "ImageSet.h"
#include "NavigationMap.h"
#include "Font.h"

typedef struct Location Location;

enum ElementAction {
    ElementActionIdle,
    ElementActionMove,
    ElementActionTalk,
    ElementActionAnimate
};

typedef struct Element {
    int id;
    Location *location;
    Vector position;
    bool isSelectable;
    SDL_Rect selectionRect;
    Vector target;
    bool isVisible;
    char name[ELEMENT_NAME_SIZE];
    struct Image *image;
    int frameIndex;
    int frameTicks;
    int loopCount;
    bool isPaused;
    struct ImageSet *imageSet;
    int imageId;
    Vector direction;
    enum ElementAction action;
    NavigationPath *navigationPath;
    bool navigationIgnoresMap;
    int navigationIndex;
    float speed;
    Image *talkImage;
    Vector talkOffset;
    int talkTicks;
    int defaultIdleImageId;
    int defaultWalkImageId;
    struct Element *next;
} Element;

Element *CreateElement(int id);
void FreeElement(Element *element);
void UpdateElement(Element *element, int deltaTicks);
void DrawElement(Element *element);
void DrawElementOverlay(Element *element);
bool IsPointInElement(Element *element, int x, int y);
void ResetDefaultAnimations(Element *element);
Vector GetElementTarget(Element *element, Vector fromPosition);

void ElementStop(Element *element);
void ElementSetSide(Element *element, ImageSide side, int imageId);
void ElementLookTo(Element *element, int x, int y, int imageId);
void ElementMoveTo(Element *element, int x, int y, int imageId, bool ignoreNavMap);
void ElementTalk(Element *element, const char *text, int imageId, Font *font);
void ElementAnimate(Element *element, int imageId, int loopCount);

#endif /* Element_h */
