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

#ifndef InventoryBar_h
#define InventoryBar_h

#include <stdio.h>
#include <stdbool.h>
#include "GameState.h"
#include "Utils.h"
#include "Image.h"
#include "Enums.h"

typedef enum InventoryBarButton {
    InventoryBarButtonNone = -1,
    InventoryBarButtonMenu,
    InventoryBarButtonUp,
    InventoryBarButtonDown
} InventoryBarButton;

typedef struct InventoryItemView {
    InventoryItem *item;
    Vector position;
} InventoryItemView;

typedef struct InventoryBar {
    GameState *gameState;
    Image *image;
#ifdef TOUCH
    Image *buttonLeftImage;
    Image *buttonRightImage;
    Image *menuButtonImage;
#else
    Image *buttonsImage;
#endif
    InventoryItemView itemViews[INVENTORY_BAR_SIZE];
    bool isVisible;
    bool isEnabled;
    InventoryBarButton focusedButton;
    int firstItemIndex;
    int scrollTimer;
} InventoryBar;

InventoryBar *CreateInventoryBar(GameState *gameState);
void FreeInventoryBar(InventoryBar *bar);
void RefreshInventoryBar(InventoryBar *bar, bool showNew);
bool HandleMouseInInventoryBar(InventoryBar *bar, int x, int y, ButtonState buttonState, bool isDraggingItem);
void UpdateInventoryBar(InventoryBar *bar, int deltaTicks);
void DrawInventoryBar(InventoryBar *bar);

InventoryItem *GetItemInInventoryBarAt(InventoryBar *bar, int x, int y);

void DrawInventoryItemView(InventoryItemView *itemView);

#endif /* InventoryBar_h */
