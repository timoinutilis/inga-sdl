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

#ifndef Menu_h
#define Menu_h

#include <stdio.h>
#include <stdbool.h>
#include "Utils.h"
#include "Image.h"
#include "Config.h"
#include "Fader.h"
#include "Enums.h"

typedef struct Game Game;

typedef struct MenuItem {
    int id;
    const char *text;
    Image *image;
    Image *focusImage;
    Vector position;
    struct MenuItem *next;
} MenuItem;

typedef struct Menu {
    Game *game;
    Image *image;
    Image *itemImage;
    Image *titleImage;
    Image *versionImage;
    Vector titlePosition;
    MenuItem *rootItem;
    MenuItem *lastItem;
    MenuItem *focusedItem;
    int frameIndex;
    int frameTicks;
    Fader fader;
} Menu;

Menu *CreateMenu(Game *game);
void FreeMenu(Menu *menu);
void OpenMenu(Menu *menu, int startItem);
void CloseMenu(Menu *menu);
bool HandleMouseInMenu(Menu *menu, int x, int y, ButtonState buttonState);
bool UpdateMenu(Menu *menu, int deltaTicks);
bool DrawMenu(Menu *menu);

#endif /* Menu_h */
