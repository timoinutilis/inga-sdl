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

#include "InventoryBar.h"
#include "Global.h"

InventoryBar *CreateInventoryBar(GameState *gameState) {
    InventoryBar *bar = calloc(1, sizeof(InventoryBar));
    if (!bar) {
        printf("CreateInventoryBar: Out of memory\n");
    } else {
        bar->gameState = gameState;
        bar->image = LoadImage("Inventarleiste", GetGlobalPalette(), false, false);
        for (int i = 0; i < INVENTORY_BAR_SIZE; i++) {
            bar->itemViews[i].position = MakeVector(75 + (i * 71), SCREEN_HEIGHT - 68);
        }
    }
    return bar;
}

void FreeInventoryBar(InventoryBar *bar) {
    if (!bar) return;
    FreeImage(bar->image);
    free(bar);
}

bool HandleMouseInInventoryBar(InventoryBar *bar, int x, int y, int buttonIndex) {
    if (!bar) return false;
    if (bar->isVisible) {
        if (y < SCREEN_HEIGHT - bar->image->height) {
            bar->isVisible = false;
            return false;
        }
        return true;
    } else {
        if (y >= SCREEN_HEIGHT - 16) {
            bar->isVisible = true;
            return true;
        }
        return false;
    }
}

void UpdateInventoryBar(InventoryBar *bar, int deltaTicks) {
    if (!bar) return;
    InventoryItem *item = bar->gameState->rootInventoryItem;
    for (int i = 0; i < INVENTORY_BAR_SIZE; i++) {
        if (item != bar->itemViews[i].item) {
            if (item) {
                //TODO: reset animation
            }
            bar->itemViews[i].item = item;
        }
        if (item) {
            item = item->next;
        }
    }
}

void DrawInventoryBar(InventoryBar *bar) {
    if (!bar || !bar->isVisible) return;
    DrawImage(bar->image, MakeVector(0, SCREEN_HEIGHT - bar->image->height));
    for (int i = 0; i < INVENTORY_BAR_SIZE; i++) {
        InventoryItemView *itemView = &bar->itemViews[i];
        if (itemView->item && itemView->item->image) {
            if (itemView->item->image->animation) {
                DrawAnimationFrame(itemView->item->image, itemView->position, 0);
            } else {
                DrawImage(itemView->item->image, itemView->position);
            }
        }
    }
}

InventoryItem *GetItemInInventoryBarAt(InventoryBar *bar, int x, int y) {
    if (!bar || !bar->isVisible) return NULL;
    for (int i = 0; i < INVENTORY_BAR_SIZE; i++) {
        Vector position = bar->itemViews[i].position;
        if (x >= position.x && y >= position.y && x < position.x + 48 && y < position.y + 48) {
            return bar->itemViews[i].item;
        }
    }
    return NULL;
}
