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

#include "Game.h"
#include "Global.h"
#include "Cursor.h"

void SetFocus(Game *game, int x, int y, const char *name);
void UpdateIdleProg(Game *game, int deltaTicks);

Game *CreateGame(GameConfig *config) {
    Game *game = calloc(1, sizeof(Game));
    if (!game) {
        printf("CreateGame: Out of memory\n");
    } else {
        game->config = config;
        game->font = LoadFont("Font", 16);
        game->cursorNormal = LoadCursor("CursorNormal");
        game->cursorDrag = LoadCursor("CursorDrag");
        game->script = LoadScript("story");
        game->gameState = CreateGameState();
        game->inventoryBar = CreateInventoryBar(game->gameState);
        game->dialog = CreateDialog();
        game->mainThread = CreateThread(0);
        game->escImage = LoadImage("Esc", GetGlobalPalette(), true, false);
        game->menu = CreateMenu(game);
        game->slotList = CreateSlotList(config);
        game->soundManager = CreateSoundManager();
        InitFader(&game->fader, FADE_DURATION);
        
        // Main Person
        Element *element = CreateElement(MainPersonID);
        element->layer = LayerPersons;
        element->imageSet = LoadImageSet("Hauptperson", GetGlobalPalette(), true);
        game->mainPerson = element;
        
        HideCursor();
    }
    return game;
}

void FreeGame(Game *game) {
    if (!game) return;
    FreeSoundManager(game->soundManager);
    FreeSlotList(game->slotList);
    FreeMenu(game->menu);
    FreeCursor(game->cursorNormal);
    FreeCursor(game->cursorDrag);
    FreeImage(game->escImage);
    FreeImage(game->focus.image);
    FreeSequence(game->sequence);
    FreeLocation(game->location);
    FreeElement(game->mainPerson);
    FreeThread(game->mainThread);
    FreeDialog(game->dialog);
    FreeInventoryBar(game->inventoryBar);
    FreeGameState(game->gameState);
    FreeScript(game->script);
    FreeFont(game->font);
    free(game);
}

void HandleMouseInGame(Game *game, int x, int y, int buttonIndex) {
    if (!game) return;
    
    if (HandleMouseInMenu(game->menu, x, y, buttonIndex)) {
        SetFocus(game, x, y, NULL);
        game->draggingItemView.item = NULL;
        return;
    }
    
    if (HandleMouseInSequence(game->sequence, x, y, buttonIndex)) {
        SetFocus(game, x, y, NULL);
        game->draggingItemView.item = NULL;
        return;
    }
    
    if (game->mainThread && game->mainThread->isActive) {
        SetFocus(game, x, y, NULL);
        game->draggingItemView.item = NULL;
        return;
    }
    
    if (HandleMouseInDialog(game->dialog, x, y, buttonIndex)) {
        SetFocus(game, x, y, NULL);
        game->draggingItemView.item = NULL;
        if (buttonIndex == SDL_BUTTON_LEFT && game->dialog->focusedItem) {
            StartInteraction(game->mainThread, game->dialog->focusedItem->id, 0, VerbSay);
            ResetDialog(game->dialog);
        }
        return;
    }
    
    game->draggingItemView.position = MakeVector(x, y);
    
    if (HandleMouseInInventoryBar(game->inventoryBar, x, y, buttonIndex)) {
        if (game->inventoryBar->focusedButton != InventoryBarButtonNone) {
            SetFocus(game, x, y, NULL);
            if (buttonIndex == SDL_BUTTON_LEFT) {
                if (game->inventoryBar->focusedButton == InventoryBarButtonMenu) {
                    RefreshGameState(game);
                    OpenMenu(game->menu);
                }
            }
            return;
        }
        InventoryItem *focusedItem = GetItemInInventoryBarAt(game->inventoryBar, x, y);
        if (focusedItem) {
            SetFocus(game, x, y, focusedItem->name);
            if (buttonIndex == SDL_BUTTON_LEFT) {
                if (game->draggingItemView.item) {
                    StartInteraction(game->mainThread, focusedItem->id, game->draggingItemView.item->id, VerbUse);
                    game->draggingItemView.item = NULL;
                    game->inventoryBar->isVisible = false;
                } else {
                    SetCursor(game->cursorDrag);
                    game->draggingItemView.item = focusedItem;
                }
            } else if (buttonIndex == SDL_BUTTON_RIGHT) {
                game->draggingItemView.item = NULL;
                StartInteraction(game->mainThread, focusedItem->id, 0, VerbLook);
            }
        } else {
            SetFocus(game, x, y, NULL);
        }
        return;
    }
    
    if (!game->location) return;
    
    Element *focusedElement = GetElementAt(game->location, x, y);
    if (focusedElement) {
        SetFocus(game, x, y, focusedElement->name);
        if (buttonIndex > 0) {
            game->selectedId = focusedElement->id;
            if (game->draggingItemView.item) {
                game->draggedId = game->draggingItemView.item->id;
                game->selectedVerb = VerbUse;
                game->draggingItemView.item = NULL;
                SetCursor(game->cursorNormal);
            } else {
                game->selectedVerb = buttonIndex == SDL_BUTTON_RIGHT ? VerbLook : VerbUse;
            }
            Thread *thread = GetThread(game->location, focusedElement->id);
            if (thread) {
                thread->isActive = false;
            }
            ElementStop(focusedElement);
            Element *person = GetElement(game->location, MainPersonID);
            if (!person->isVisible) {
                MainPersonDidFinishWalking(game);
            } else {
                Vector target = GetElementTarget(focusedElement, person->position);
                ElementMoveTo(person, target.x, target.y, 0, false);
            }
        }
        return;
    }
    
    SetFocus(game, x, y, NULL);
    
    if (buttonIndex == SDL_BUTTON_LEFT) {
        game->selectedId = 0;
        Element *person = GetElement(game->location, MainPersonID);
        ElementMoveTo(person, x, y, 0, false);
    }
}

void HandleKeyInGame(Game *game, SDL_Keysym keysym) {
    if (!game) return;
    
    if (game->menu->fader.state != FaderStateClosed) {
        return;
    }
    
    if (game->sequence) {
        return;
    }
    
    if (keysym.sym == SDLK_ESCAPE) {
        EscapeThread(game->mainThread);
    }
}

void HandleGameCheat(Game *game, const char *cheat) {
    if (!game || !cheat || strlen(cheat) < 3) return;
    
    if (cheat[0] == 'j' && cheat[1] == 'j' && cheat[2] == 'j') {
        long ptr = atol(cheat + 3);
        if (ptr >= 0) {
            printf("jump %ld\n", ptr);
            RunThread(game->mainThread, ptr);
        }
    } else if (cheat[0] == 'v' && cheat[1] == 'v' && cheat[2] == 'v') {
        int id = atoi(cheat + 3);
        if (id > 0) {
            char *s = strchr(cheat, ' ');
            if (s) {
                int value = atoi(s);
                if (value > 0) {
                    printf("variable %d = %d\n", id, value);
                    SetVariable(game->gameState, id, value, false);
                }
            }
        }
    } else if (cheat[0] == 'i' && cheat[1] == 'i' && cheat[2] == 'i') {
        int id = atoi(cheat + 3);
        if (id >= 0) {
            printf("inventory item %d\n", id);
            char name[30];
            sprintf(name, "Test %d", id);
            AddInventoryItem(game->gameState, id, name, "Esc", false); // "Esc" is one of the few small standard images
            RefreshInventoryBar(game->inventoryBar, true);
        }
    }
}

void UpdateGame(Game *game, int deltaTicks) {
    if (!game) return;
    
    if (UpdateMenu(game->menu, deltaTicks)) {
        game->inventoryBar->isVisible = false;
        return;
    }
    
    UpdatePlaytime(game->gameState, deltaTicks);
    
    if (game->sequence) {
        UpdateSequence(game->sequence, deltaTicks);
        if (game->sequence->isFinished) {
            FreeSequence(game->sequence);
            game->sequence = NULL;
        } else {
            return;
        }
    }
    
    UpdateThread(game->mainThread, game);
    UpdateLocation(game->location, deltaTicks);
    UpdateInventoryBar(game->inventoryBar, deltaTicks);
    UpdateIdleProg(game, deltaTicks);
    UpdateFader(&game->fader, deltaTicks);
}

void DrawGame(Game *game) {
    if (!game) return;
    
    if (DrawMenu(game->menu)) {
        return;
    }
    
    if (game->sequence) {
        DrawSequence(game->sequence);
        return;
    }
    
    DrawLocation(game->location);
    DrawInventoryBar(game->inventoryBar);
    DrawImage(game->focus.image, game->focus.position);
    DrawInventoryItemView(&game->draggingItemView);
    DrawDialog(game->dialog);
    DrawFader(&game->fader);
    
    if (game->mainThread->escptr) {
        DrawImage(game->escImage, MakeVector(1, 1));
    }
}

void SetFocus(Game *game, int x, int y, const char *name) {
    if (name != game->focus.name) {
        FreeImage(game->focus.image);
        game->focus.image = NULL;
        game->focus.name = name;
        if (name && name[0] != 0) {
            SDL_Color color = {255, 255, 0, 255};
            game->focus.image = CreateImageFromText(name, game->font, color);
        }
    }
    if (game->focus.image) {
        int width = game->focus.image->width;
        game->focus.position = MakeVector(fmin(fmax(0, x - width * 0.5), SCREEN_WIDTH - width), y - game->focus.image->height - 4);
    }
}

void UpdateIdleProg(Game *game, int deltaTicks) {
    if (game->mainPerson && game->idleScript.delay && game->mainPerson->action == ElementActionIdle && !game->mainThread->isActive && !game->draggingItemView.item) {
        game->idleScript.idleTicks += deltaTicks;
        if (game->idleScript.idleTicks >= game->idleScript.delay) {
            RunThread(game->mainThread, game->idleScript.scriptPtr);
            game->idleScript.idleTicks = 0;
        }
    } else {
        game->idleScript.idleTicks = 0;
    }
}

void SetLocation(Game *game, int id, const char *background) {
    game->mainThread->talkingElement = NULL;
    ElementStop(game->mainPerson);
    game->mainPerson->isVisible = true;
    ResetDefaultAnimations(game->mainPerson);
    FreeLocation(game->location);
    
    game->location = CreateLocation(id, background);
    game->location->game = game;
    
    AddElement(game->location, game->mainPerson);
    game->mainPerson->position = game->gameState->startPosition;
    ElementSetDirection(game->mainPerson, game->gameState->startDirection);
    
    game->inventoryBar->isEnabled = true;
    game->inventoryBar->isVisible = false;
}

void SetGameState(Game *game, GameState *gameState) {
    if (!game || !gameState) return;
    FreeGameState(game->gameState);
    game->gameState = gameState;
    game->inventoryBar->gameState = gameState;
    RefreshInventoryBar(game->inventoryBar, true);
    Label *label = GetLabelWithName(game->script, gameState->locationLabel);
    RunThread(game->mainThread, label ? label->ptr : 0);
    game->fader.state = FaderStateClosed;
}

void RefreshGameState(Game *game) {
    if (!game || !game->gameState) return;
    game->gameState->startPosition = game->mainPerson->position;
    game->gameState->startDirection = game->mainPerson->direction;
}

void MainPersonDidFinishWalking(Game *game) {
    if (game->selectedId) {
        StartInteraction(game->mainThread, game->selectedId, game->draggedId, game->selectedVerb);
        game->selectedId = 0;
        game->draggedId = 0;
    }
}
