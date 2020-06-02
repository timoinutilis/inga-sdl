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
        bar->buttonsImage = LoadImage("InventarleisteKnoepfe", GetGlobalPalette(), false, false);
        bar->focusedButton = InventoryBarButtonNone;
        for (int i = 0; i < INVENTORY_BAR_SIZE; i++) {
            bar->itemViews[i].position = MakeVector(75 + (i * 71), SCREEN_HEIGHT - 68);
        }
        bar->isEnabled = true;
    }
    return bar;
}

void FreeInventoryBar(InventoryBar *bar) {
    if (!bar) return;
    FreeImage(bar->image);
    FreeImage(bar->buttonsImage);
    free(bar);
}

void RefreshInventoryBar(InventoryBar *bar, bool showNew) {
    if (!bar) return;
    InventoryItem *item = bar->gameState->rootInventoryItem;
    if (showNew) {
        bar->firstItemIndex = 0;
    }
    for (int i = 0; i < bar->firstItemIndex && item; i++) {
        item = item->next;
    }
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

bool HandleMouseInInventoryBar(InventoryBar *bar, int x, int y, int buttonIndex) {
    if (!bar || !bar->image || !bar->buttonsImage) return false;
    bar->focusedButton = InventoryBarButtonNone;
    if (bar->isVisible) {
        int barY = SCREEN_HEIGHT - bar->image->height;
        if (y < barY) {
            bar->isVisible = false;
            return false;
        }
        if (x < 56) {
            if (y >= barY && y < barY + 30) {
                bar->focusedButton = InventoryBarButtonMenu;
            } else if (y >= barY + 38 && y < barY + 52) {
                if (bar->firstItemIndex > 0) {
                    bar->focusedButton = InventoryBarButtonUp;
                }
            } else if (y >= barY + 55 && y < barY + 70) {
                if (bar->firstItemIndex + INVENTORY_BAR_SIZE < bar->gameState->numInventoryItems) {
                    bar->focusedButton = InventoryBarButtonDown;
                }
            }
        }
        if (buttonIndex == SDL_BUTTON_LEFT) {
            switch (bar->focusedButton) {
                case InventoryBarButtonNone:
                    break;
                case InventoryBarButtonMenu:
                    break;
                case InventoryBarButtonUp:
                    bar->firstItemIndex -= INVENTORY_BAR_SIZE;
                    RefreshInventoryBar(bar, false);
                    break;
                case InventoryBarButtonDown:
                    bar->firstItemIndex += INVENTORY_BAR_SIZE;
                    RefreshInventoryBar(bar, false);
                    break;
            }
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
    if (bar->isVisible && !bar->isEnabled) {
        bar->isVisible = false;
    }
}

void DrawInventoryBar(InventoryBar *bar) {
    if (!bar || !bar->isVisible) return;
    Vector position = MakeVector(0, SCREEN_HEIGHT - bar->image->height);
    DrawImage(bar->image, position);
    if (bar->focusedButton != InventoryBarButtonNone) {
        DrawAnimationFrame(bar->buttonsImage, position, bar->focusedButton);
    }
    for (int i = 0; i < INVENTORY_BAR_SIZE; i++) {
        DrawInventoryItemView(&bar->itemViews[i]);
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

void DrawInventoryItemView(InventoryItemView *itemView) {
    if (!itemView || !itemView->item || !itemView->item->image) return;
    if (itemView->item->image->animation) {
        DrawAnimationFrame(itemView->item->image, itemView->position, 0);
    } else {
        DrawImage(itemView->item->image, itemView->position);
    }
}
