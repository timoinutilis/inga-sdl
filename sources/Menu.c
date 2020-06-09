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

#include "Menu.h"
#include "Game.h"
#include "Font.h"

void AddMenuItem(Menu *menu, int id, const char *text);
void RefreshMenu(Menu *menu);
void ResetMenu(Menu *menu);


Menu *CreateMenu(Game *game) {
    Menu *menu = calloc(1, sizeof(Menu));
    if (!menu) {
        printf("CreateMenu: Out of memory\n");
    } else {
        menu->game = game;
        menu->image = LoadImage("Menue", NULL, false, true);
        menu->itemImage = LoadImage("Menuepunkt", menu->image->surface->format->palette, true, false);
    }
    return menu;
}

void FreeMenu(Menu *menu) {
    if (!menu) return;
    ResetMenu(menu);
    FreeImage(menu->image);
    FreeImage(menu->itemImage);
    free(menu);
}

void OpenMenu(Menu *menu) {
    if (!menu) return;
    menu->isOpen = true;
    
    AddMenuItem(menu, 1, "Spiel neu beginnen");
    AddMenuItem(menu, 2, "Spielstand laden");
    AddMenuItem(menu, 3, "Spielstand speichern");
    AddMenuItem(menu, 4, "Weiterspielen");
    AddMenuItem(menu, 5, "Spiel beenden");
    RefreshMenu(menu);
}

void CloseMenu(Menu *menu) {
    if (!menu) return;
    menu->isOpen = false;
    ResetMenu(menu);
}

bool HandleMouseInMenu(Menu *menu, int x, int y, int buttonIndex) {
    if (!menu || !menu->isOpen) return false;
        
    menu->focusedItem = NULL;
    MenuItem *item = menu->rootItem;
    while (item) {
        if (y >= item->position.y && y < item->position.y + item->image->height) {
            menu->focusedItem = item;
            break;
        }
        item = item->next;
    }
    
    if (buttonIndex == SDL_BUTTON_LEFT && menu->focusedItem) {
        switch (menu->focusedItem->id) {
            case 4:
                CloseMenu(menu);
                break;
        }
    }
    
    return true;
}

bool UpdateMenu(Menu *menu, int deltaTicks) {
    if (!menu || !menu->isOpen) return false;
    return true;
}

bool DrawMenu(Menu *menu) {
    if (!menu || !menu->isOpen) return false;
    DrawImage(menu->image, MakeVector(0, 0));
    
    MenuItem *item = menu->rootItem;
    while (item) {
        if (item == menu->focusedItem) {
            DrawImage(item->focusImage, item->position);
        } else {
            DrawImage(item->image, item->position);
        }
        item = item->next;
    }
    
    return true;
}

void AddMenuItem(Menu *menu, int id, const char *text) {
    if (!menu) return;
    MenuItem *item = calloc(1, sizeof(MenuItem));
    if (!item) {
        printf("AddMenuItem: Out of memory\n");
    } else {
        SDL_Color color = {132, 161, 235, 255};
        SDL_Color focusColor = {255, 255, 255, 255};
        item->id = id;
        item->text = text;
        item->image = CreateImageFromText(text, menu->game->font, color);
        item->focusImage = CreateImageFromText(text, menu->game->font, focusColor);
        item->next = menu->rootItem;
        menu->rootItem = item;
    }
}

void RefreshMenu(Menu *menu) {
    if (!menu || !menu->rootItem) return;
    MenuItem *item = menu->rootItem;
    Vector position = MakeVector(0, 0);
    while (item) {
        position.x = 260;
        item->position = position;
        position.y += item->image->height + 4;
        item = item->next;
    }
    float offsetY = (SCREEN_HEIGHT - position.y) * 0.5f;
    item = menu->rootItem;
    while (item) {
        item->position.y += offsetY;
        item = item->next;
    }
}

void ResetMenu(Menu *menu) {
    if (!menu) return;
    MenuItem *item = menu->rootItem;
    while (item) {
        MenuItem *next = item->next;
        FreeImage(item->image);
        FreeImage(item->focusImage);
        free(item);
        item = next;
    }
    menu->rootItem = NULL;
    menu->focusedItem = NULL;
}
