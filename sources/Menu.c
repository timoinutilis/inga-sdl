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
#include "Global.h"

void AddMenuItem(Menu *menu, int id, const char *text);
void RefreshMenu(Menu *menu);
void ResetMenu(Menu *menu);
void HandleMenuItem(Menu *menu, int id);

const char *MenuTextSpeed[] = {
    "text_speed_fast",
    "text_speed_medium",
    "text_speed_slow"
};


Menu *CreateMenu(Game *game) {
    Menu *menu = calloc(1, sizeof(Menu));
    if (!menu) {
        printf("CreateMenu: Out of memory\n");
    } else {
        menu->game = game;
        menu->image = LoadImage("Menue", NULL, false, true);
        menu->itemImage = LoadImage("Menuepunkt", menu->image->surface->format->palette, true, false);
        menu->frameIndex = 1;
        
        SDL_Color color = {255, 255, 255, 255};
        menu->versionImage = CreateImageFromText(game->config->version, menu->game->font, color);
        
        InitFader(&menu->fader, FADE_DURATION);
    }
    return menu;
}

void FreeMenu(Menu *menu) {
    if (!menu) return;
    ResetMenu(menu);
    FreeImage(menu->image);
    FreeImage(menu->itemImage);
    FreeImage(menu->versionImage);
    free(menu);
}

void OpenMenu(Menu *menu, int startItem) {
    if (!menu) return;
    HandleMenuItem(menu, startItem);
    SDL_SetCursor(menu->game->cursorNormal);
    FadeIn(&menu->fader);
}

void CloseMenu(Menu *menu) {
    if (!menu) return;
    ResetMenu(menu);
    FadeOut(&menu->fader);
}

bool HandleMouseInMenu(Menu *menu, const int x, const int y, const ButtonState buttonState) {
    if (!menu || menu->fader.state == FaderStateClosed) return false;
    menu->focusedItem = NULL;
    if (CanHover(buttonState)) {
        MenuItem *item = menu->rootItem;
        while (item) {
            if (y >= (int)item->position.y - 8 && y < (int)item->position.y + item->image->height + 8) {
                menu->focusedItem = item;
                break;
            }
            item = item->next;
        }
        if (buttonState == SelectionButtonState() && menu->focusedItem) {
            HandleMenuItem(menu, menu->focusedItem->id);
        }
    }
    return true;
}

bool UpdateMenu(Menu *menu, int deltaTicks) {
    if (!menu || menu->fader.state == FaderStateClosed) return false;
    
    UpdateFader(&menu->fader, deltaTicks);
    
    // Animation
    if (menu->itemImage && menu->itemImage->animation) {
        menu->frameTicks += deltaTicks;
        if (menu->frameTicks >= menu->itemImage->animation->frames[menu->frameIndex].ticks) {
            menu->frameTicks = 0;
            menu->frameIndex += 1;
            int numFrames = menu->itemImage->animation->numFrames;
            if (menu->frameIndex >= numFrames) {
                menu->frameIndex = 1; // frame 0 is for not highlighted items
            }
        }
    }
    
    return true;
}

bool DrawMenu(Menu *menu) {
    if (!menu || menu->fader.state == FaderStateClosed) return false;
    DrawImage(menu->image, MakeVector(0, 0));
    
    DrawImage(menu->titleImage, menu->titlePosition);
    
    MenuItem *item = menu->rootItem;
    while (item) {
        Vector iconPosition = MakeVector(210, item->position.y);
        if (item == menu->focusedItem) {
            DrawAnimationFrame(menu->itemImage, iconPosition, menu->frameIndex);
            DrawImage(item->focusImage, item->position);
        } else {
            DrawAnimationFrame(menu->itemImage, iconPosition, 0);
            DrawImage(item->image, item->position);
        }
        item = item->next;
    }
    
    DrawImage(menu->versionImage, MakeVector(634 - menu->versionImage->width, 478 - menu->versionImage->height));
    
    DrawFader(&menu->fader);
    
    return true;
}

void SetMenuTitle(Menu *menu, const char *text) {
    if (!menu) return;
    SDL_Color color = {255, 255, 255, 255};
    menu->titleImage = CreateImageFromText(text, menu->game->font, color);
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
        if (menu->lastItem) {
            menu->lastItem->next = item;
        } else {
            menu->rootItem = item;
        }
        menu->lastItem = item;
    }
}

void RefreshMenu(Menu *menu) {
    if (!menu || !menu->rootItem) return;
    MenuItem *item = menu->rootItem;
    Vector position = MakeVector(0, 0);
    while (item) {
        position.x = 260;
        item->position = position;
        position.y += item->image->height + 16;
        item = item->next;
    }
    float offsetY = (SCREEN_HEIGHT - position.y) * 0.5f;
    item = menu->rootItem;
    while (item) {
        item->position.y += offsetY;
        item = item->next;
    }
    if (menu->titleImage) {
        menu->titlePosition = MakeVector(220, offsetY - menu->titleImage->height - 16);
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
    menu->lastItem = NULL;
    menu->focusedItem = NULL;
    FreeImage(menu->titleImage);
    menu->titleImage = NULL;
}

void HandleMenuItem(Menu *menu, int id) {
    ResetMenu(menu);
    Game *game = menu->game;
    switch (id) {
        case 0:
            SetMenuTitle(menu, GetText(game, "main_menu_title"));
            AddMenuItem(menu, 4, GetText(game, "resume_playing"));
            AddMenuItem(menu, 2, GetText(game, "load_game"));
            AddMenuItem(menu, 3, GetText(game, "save_game"));
            AddMenuItem(menu, 6, GetText(game, MenuTextSpeed[game->gameState->textSpeed]));
            if (game->config->numLanguages > 0) {
                AddMenuItem(menu, 7, GetText(game, "language"));
            }
            AddMenuItem(menu, 1, GetText(game, "restart_game"));
#ifndef AUTOSAVE
            AddMenuItem(menu, 5, GetText(game, "quit_game"));
#endif
            RefreshMenu(menu);
            break;
        case 1:
            SetMenuTitle(menu, GetText(game, "restart_game_alert"));
            AddMenuItem(menu, 10, GetText(game, "restart"));
            AddMenuItem(menu, 0, GetText(game, "cancel"));
            RefreshMenu(menu);
            break;
        case 10:
            SetGameState(game, CreateGameState());
            CloseMenu(menu);
            break;
        case 2:
            SetMenuTitle(menu, GetText(game, "load_game_title"));
            for (int i = 0; i < NUM_SAVE_SLOTS; ++i) {
                AddMenuItem(menu, 20 + i, game->slotList->slotNames[i]);
            }
            AddMenuItem(menu, 0, GetText(game, "cancel"));
            RefreshMenu(menu);
            break;
        case 3:
            SetMenuTitle(menu, GetText(game, "save_game_title"));
            for (int i = 0; i < NUM_SAVE_SLOTS; ++i) {
                AddMenuItem(menu, 30 + i, game->slotList->slotNames[i]);
            }
            AddMenuItem(menu, 0, GetText(game, "cancel"));
            RefreshMenu(menu);
            break;
        case 4:
            CloseMenu(menu);
            break;
        case 5:
            SetMenuTitle(menu, GetText(game, "quit_game_alert"));
            if (game->gameState->hasChangedSinceSave) {
                AddMenuItem(menu, 3, GetText(game, "save_game"));
            }
            AddMenuItem(menu, 50, GetText(game, "quit"));
            AddMenuItem(menu, 0, GetText(game, "cancel"));
            RefreshMenu(menu);
            break;
        case 50:
            SetShouldQuit();
            break;
        case 6:
            ++game->gameState->textSpeed;
            if (game->gameState->textSpeed > MAX_TEXT_SPEED) {
                game->gameState->textSpeed = 0;
            }
            HandleMenuItem(menu, 0);
            break;
        case 7:
            SetMenuTitle(menu, "");
            for (int i = 0; i < game->config->numLanguages; i++) {
                AddMenuItem(menu, 70 + i, game->config->languageNames[i]);
            }
            AddMenuItem(menu, 0, GetText(game, "cancel"));
            RefreshMenu(menu);
            break;
        case 8:
            SetMenuTitle(menu, "");
            for (int i = 0; i < game->config->numLanguages; i++) {
                AddMenuItem(menu, 70 + i, game->config->languageNames[i]);
            }
            RefreshMenu(menu);
            break;
        default:
            if (id >= 20 && id < 30) {
                // load
                int slot = id - 20;
                LoadGameSlot(game, slot);
                CloseMenu(menu);
            } else if (id >= 30 && id < 40) {
                // save
                int slot = id - 30;
                SaveGameSlot(game, slot);
                SetMenuTitle(menu, GetText(game, "game_saved_message"));
                AddMenuItem(menu, 0, GetText(game, "ok"));
                RefreshMenu(menu);
            } else if (id >= 70 && id < 80) {
                int langIndex = id - 70;
                SetLanguage(game, game->config->languageCodes[langIndex]);
                CloseMenu(menu);
            }
            break;
    }

}
