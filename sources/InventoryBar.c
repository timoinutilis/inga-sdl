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

#include "InventoryBar.h"
#include "Global.h"

InventoryBar *CreateInventoryBar(GameState *gameState) {
    InventoryBar *bar = calloc(1, sizeof(InventoryBar));
    if (!bar) {
        printf("CreateInventoryBar: Out of memory\n");
    } else {
        bar->gameState = gameState;
#ifdef TOUCH
        bar->image = LoadImage("InventarleisteMobile", GetGlobalPalette(), false, false);
        bar->buttonLeftImage = LoadImage("InventarleisteMKLinks", GetGlobalPalette(), false, false);
        bar->buttonRightImage = LoadImage("InventarleisteMKRechts", GetGlobalPalette(), false, false);
        bar->menuButtonImage = LoadImage("MenuKnopf", GetGlobalPalette(), true, false);
#else
        bar->image = LoadImage("Inventarleiste", GetGlobalPalette(), false, false);
        bar->buttonsImage = LoadImage("InventarleisteKnoepfe", GetGlobalPalette(), false, false);
#endif
        bar->focusedButton = InventoryBarButtonNone;
        for (int i = 0; i < INVENTORY_BAR_SIZE; i++) {
            bar->itemViews[i].position = MakeVector(INVENTORY_ITEM_OFFSET_X + i * (INVENTORY_ITEM_WIDTH + INVENTORY_ITEM_SPACE), SCREEN_HEIGHT - INVENTORY_ITEM_OFFSET_Y);
        }
        bar->isEnabled = true;
        RefreshInventoryBar(bar, false);
    }
    return bar;
}

void FreeInventoryBar(InventoryBar *bar) {
    if (!bar) return;
    FreeImage(bar->image);
#ifdef TOUCH
    FreeImage(bar->buttonLeftImage);
    FreeImage(bar->buttonRightImage);
    FreeImage(bar->menuButtonImage);
#else
    FreeImage(bar->buttonsImage);
#endif
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

void ScrollInventoryBar(InventoryBar *bar) {
    switch (bar->focusedButton) {
        case InventoryBarButtonNone:
            break;
        case InventoryBarButtonMenu:
            break;
        case InventoryBarButtonUp:
            bar->firstItemIndex -= INVENTORY_BAR_SIZE;
            bar->scrollTimer = 0;
            RefreshInventoryBar(bar, false);
            break;
        case InventoryBarButtonDown:
            bar->firstItemIndex += INVENTORY_BAR_SIZE;
            bar->scrollTimer = 0;
            RefreshInventoryBar(bar, false);
            break;
    }
}

bool HandleMouseInInventoryBar(InventoryBar *bar, int x, int y, ButtonState buttonState, bool isDraggingItem) {
    if (!bar || !bar->image) return false;
    InventoryBarButton lastFocusedButton = bar->focusedButton;
    bar->focusedButton = InventoryBarButtonNone;
    if (bar->isVisible) {
#ifdef TOUCH
        if (buttonState != ButtonStateIdle) {
            if (x < 44 && y < 44) {
                bar->focusedButton = InventoryBarButtonMenu;
                return true;
            }
        }
#endif
        int barY = SCREEN_HEIGHT - bar->image->height;
        if (y < barY) {
            bar->isVisible = false;
            return false;
        }
#ifdef TOUCH
        if (buttonState != ButtonStateIdle) {
            if (x < 50) {
                if (bar->firstItemIndex > 0) {
                    bar->focusedButton = InventoryBarButtonUp;
                }
            } else if (x > SCREEN_WIDTH - 50) {
                if (bar->firstItemIndex + INVENTORY_BAR_SIZE < bar->gameState->numInventoryItems) {
                    bar->focusedButton = InventoryBarButtonDown;
                }
            }
        }
#else
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
#endif
        if (lastFocusedButton != bar->focusedButton || buttonState == ButtonStateIdle) {
            bar->scrollTimer = 0;
        }
#ifdef TOUCH
        if ((!isDraggingItem && buttonState == ButtonStateRelease) || (isDraggingItem && bar->scrollTimer >= 500)) {
            ScrollInventoryBar(bar);
        }
#else
        if (buttonState == ButtonStateClickLeft || (buttonState == ButtonStateDrag && bar->scrollTimer >= 500)) {
            ScrollInventoryBar(bar);
        }
#endif
        return true;
    } else {
#ifdef TOUCH
        if (buttonState == ButtonStateRelease && x < 44 && y >= SCREEN_HEIGHT - 44) {
            bar->isVisible = true;
            return true;
        }
#else
        if (y >= SCREEN_HEIGHT - 16) {
            bar->isVisible = true;
            return true;
        }
#endif
        return false;
    }
}

void UpdateInventoryBar(InventoryBar *bar, int deltaTicks) {
    if (!bar) return;
    if (bar->isVisible && !bar->isEnabled) {
        bar->isVisible = false;
        return;
    }
    bar->scrollTimer += deltaTicks;
}

void DrawInventoryBar(InventoryBar *bar) {
    if (!bar || !bar->isVisible) return;
    Vector position = MakeVector(0, SCREEN_HEIGHT - bar->image->height);
    DrawImage(bar->image, position);
#ifdef TOUCH
    DrawImage(bar->menuButtonImage, MakeVector(6, 6));
    if (bar->focusedButton == InventoryBarButtonUp) {
        DrawImage(bar->buttonLeftImage, MakeVector(0, position.y));
    } else if (bar->focusedButton == InventoryBarButtonDown) {
        DrawImage(bar->buttonRightImage, MakeVector(SCREEN_WIDTH - bar->buttonRightImage->width, position.y));
    }
#else
    if (bar->focusedButton != InventoryBarButtonNone) {
        DrawAnimationFrame(bar->buttonsImage, position, bar->focusedButton);
    }
#endif
    for (int i = 0; i < INVENTORY_BAR_SIZE; i++) {
        DrawInventoryItemView(&bar->itemViews[i]);
    }
}

InventoryItem *GetItemInInventoryBarAt(InventoryBar *bar, int x, int y) {
    if (!bar || !bar->isVisible) return NULL;
    for (int i = 0; i < INVENTORY_BAR_SIZE; i++) {
        Vector position = bar->itemViews[i].position;
        if (x >= position.x && y >= position.y && x < position.x + INVENTORY_ITEM_WIDTH && y < position.y + INVENTORY_ITEM_HEIGHT) {
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
