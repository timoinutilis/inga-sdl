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

#ifndef InventoryBar_h
#define InventoryBar_h

#include <stdio.h>
#include <stdbool.h>
#include "GameState.h"
#include "Utils.h"
#include "Image.h"

typedef struct InventoryItemView {
    InventoryItem *item;
    Vector position;
} InventoryItemView;

typedef struct InventoryBar {
    GameState *gameState;
    Image *image;
    InventoryItemView itemViews[INVENTORY_BAR_SIZE];
    bool isVisible;
} InventoryBar;

InventoryBar *CreateInventoryBar(GameState *gameState);
void FreeInventoryBar(InventoryBar *bar);
void RefreshInventoryBar(InventoryBar *bar);
bool HandleMouseInInventoryBar(InventoryBar *bar, int x, int y, int buttonIndex);
void UpdateInventoryBar(InventoryBar *bar, int deltaTicks);
void DrawInventoryBar(InventoryBar *bar);

InventoryItem *GetItemInInventoryBarAt(InventoryBar *bar, int x, int y);

void DrawInventoryItemView(InventoryItemView *itemView);

#endif /* InventoryBar_h */
