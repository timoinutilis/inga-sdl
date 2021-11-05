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
#include <time.h>

void LookAtItem(Game *game, InventoryItem *focusedItem);
void DragItem(Game *game, InventoryItem *focusedItem);
void DropItem(Game *game, InventoryItem *focusedItem);
void SetFocus(Game *game, int x, int y, const char *name);
void UpdateIdleProg(Game *game, int deltaTicks);
void DoGameAction(Game *game, GameAction action);
void InitLogger(Game *game);

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
#ifdef AUTOSAVE
        GameState *autosaveGameState = LoadGameState("slot_0", config);
        game->gameState = autosaveGameState ? autosaveGameState : CreateGameState();
#else
        game->gameState = CreateGameState();
#endif
        game->inventoryBar = CreateInventoryBar(game->gameState);
        game->dialog = CreateDialog();
        game->mainThread = CreateThread(0);
#ifdef TOUCH
        game->inventoryButtonImage = LoadImage("InventarKnopf", GetGlobalPalette(), true, false);
#endif
        game->menu = CreateMenu(game);
        game->slotList = CreateSlotList(config);
        game->soundManager = CreateSoundManager();
        InitFader(&game->fader, FADE_DURATION);
        InitLogger(game);
        
        // Main Person
        Element *element = CreateElement(MainPersonID);
        element->layer = LayerPersons;
        element->imageSet = LoadImageSet("Hauptperson", GetGlobalPalette(), true);
        game->mainPerson = element;
        
        SDL_Color pauseColor = {255, 192, 0, 255};
        game->pauseImage = CreateImageFromText("Spielpause", game->font, pauseColor);
        
        HideCursor();
        
        Label *label = GetLabelWithName(game->script, game->gameState->locationLabel);
        if (label) {
            RunThread(game->mainThread, label->ptr);
        }
    }
    return game;
}

void FreeGame(Game *game) {
    if (!game) return;
    if (game->logFile) {
        fclose(game->logFile);
    }
    FreeSoundManager(game->soundManager);
    FreeSlotList(game->slotList);
    FreeMenu(game->menu);
    FreeCursor(game->cursorNormal);
    FreeCursor(game->cursorDrag);
#ifdef TOUCH
    FreeImage(game->inventoryButtonImage);
#endif
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

void HandleMouseInGame(Game *game, int x, int y, ButtonState buttonState) {
    if (!game) return;
    
    if (game->isPaused) {
        if (buttonState == ButtonStateRelease) {
            game->isPaused = false;
        }
        return;
    }
    
    if (HandleMouseInMenu(game->menu, x, y, buttonState)) {
        SetFocus(game, x, y, NULL);
        game->draggingItemView.item = NULL;
        return;
    }
    
    if (HandleMouseInSequence(game->sequence, x, y, buttonState)) {
        SetFocus(game, x, y, NULL);
        game->draggingItemView.item = NULL;
        return;
    }
    
    if (game->mainThread && game->mainThread->isActive) {
        if (buttonState == SelectionButtonState()) {
            EscapeThread(game->mainThread);
        }
        SetFocus(game, x, y, NULL);
        game->draggingItemView.item = NULL;
        return;
    }
    
    if (game->fader.state != FaderStateOpen) {
        return;
    }
    
    if (HandleMouseInDialog(game->dialog, x, y, buttonState)) {
        SetFocus(game, x, y, NULL);
        game->draggingItemView.item = NULL;
        if (buttonState == SelectionButtonState() && game->dialog->focusedItem) {
            if (game->logFile) {
                fprintf(game->logFile, "Say '%s'\n", game->dialog->focusedItem->text);
            }
            StartInteraction(game->mainThread, game->dialog->focusedItem->id, 0, VerbSay);
            ResetDialog(game->dialog);
        }
        return;
    }
    
#if TOUCH
    game->draggingItemView.position = MakeVector(x - 24, y - 100);
#else
    game->draggingItemView.position = MakeVector(x, y);
#endif
    
    if (HandleMouseInInventoryBar(game->inventoryBar, x, y, buttonState, game->draggingItemView.item != NULL)) {
#if TOUCH
        if (buttonState != ButtonStateIdle) {
            InventoryItem *focusedItem = GetItemInInventoryBarAt(game->inventoryBar, x, y);
            if (focusedItem) {
                SetFocus(game, x, y, focusedItem->name);
                if (buttonState == ButtonStateClickLeft) {
                    DragItem(game, focusedItem);
                }
            } else {
                SetFocus(game, x, y, NULL);
            }
            if (buttonState == ButtonStateRelease) {
                DropItem(game, focusedItem);
                SetFocus(game, x, y, NULL);
            }
        }
#else
        InventoryItem *focusedItem = GetItemInInventoryBarAt(game->inventoryBar, x, y);
        if (focusedItem) {
            SetFocus(game, x, y, focusedItem->name);
            if (buttonState == ButtonStateClickLeft) {
                if (game->draggingItemView.item) {
                    DropItem(game, focusedItem);
                } else {
                    DragItem(game, focusedItem);
                }
            } else if (buttonState == ButtonStateClickRight) {
                LookAtItem(game, focusedItem);
            }
        } else {
            SetFocus(game, x, y, NULL);
        }
#endif
        
        if (buttonState == SelectionButtonState()) {
            if (game->inventoryBar->focusedButton == InventoryBarButtonMenu) {
                RefreshGameState(game);
                game->actionAfterFadeOut = GameActionOpenMenu;
                FadeOut(&game->fader);
            }
        }
        return;
    }
    
    if (!game->location) return;
    
    Element *focusedElement = CanHover(buttonState) ? GetElementAt(game->location, x, y) : NULL;
    if (focusedElement) {
        SetFocus(game, x, y, focusedElement->name);
        if (buttonState == SelectionButtonState() || buttonState == ButtonStateClickRight) {
            game->selectedId = focusedElement->id;
            if (game->draggingItemView.item) {
                if (game->logFile) {
                    fprintf(game->logFile, "Use %s with %s\n", focusedElement->name, game->draggingItemView.item->name);
                }
                game->draggedId = game->draggingItemView.item->id;
                game->selectedVerb = VerbUse;
                game->draggingItemView.item = NULL;
                SetCursor(game->cursorNormal);
            } else {
                game->selectedVerb = buttonState == ButtonStateClickRight ? VerbLook : VerbUse;
                if (game->logFile) {
                    if (game->selectedVerb == VerbUse) {
                        fprintf(game->logFile, "Use %s\n", focusedElement->name);
                    } else if (game->selectedVerb == VerbLook) {
                        fprintf(game->logFile, "Look at %s\n", focusedElement->name);
                    }
                }
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
    
    if (buttonState == SelectionButtonState()) {
        game->selectedId = 0;
#if TOUCH
        if (game->draggingItemView.item) {
            game->draggingItemView.item = NULL;
            return;
        }
#endif
        Element *person = GetElement(game->location, MainPersonID);
        ElementMoveTo(person, x, y, 0, false);
    }
}

void LookAtItem(Game *game, InventoryItem *focusedItem) {
    game->draggingItemView.item = NULL;
    if (game->logFile) {
        fprintf(game->logFile, "Look at %s\n", focusedItem->name);
    }
    StartInteraction(game->mainThread, focusedItem->id, 0, VerbLook);
    game->gameState->hasChangedSinceSave = true;
}

void DragItem(Game *game, InventoryItem *focusedItem) {
    SetCursor(game->cursorDrag);
    game->draggingItemView.item = focusedItem;
}

void DropItem(Game *game, InventoryItem *focusedItem) {
    if (focusedItem && game->draggingItemView.item) {
        if (focusedItem->id == game->draggingItemView.item->id) {
            LookAtItem(game, focusedItem);
        } else {
            if (game->logFile) {
                fprintf(game->logFile, "Use %s with %s\n", focusedItem->name, game->draggingItemView.item->name);
            }
            StartInteraction(game->mainThread, focusedItem->id, game->draggingItemView.item->id, VerbUse);
            game->inventoryBar->isVisible = false;
            game->gameState->hasChangedSinceSave = true;
        }
    }
    game->draggingItemView.item = NULL;
}

void HandleKeyInGame(Game *game, SDL_Keysym keysym) {
    if (!game) return;
    
    if (keysym.sym == SDLK_SPACE) {
        game->isPaused = !game->isPaused;
    }
    
    if (game->isPaused) return;
    
    if (HandleKeyInSequence(game->sequence, keysym)) {
        return;
    }
    
    if (game->fader.state != FaderStateOpen) {
        return;
    }
    
    if (keysym.sym == SDLK_PERIOD) {
        if (game->mainThread && game->mainThread->talkingElement) {
            ElementSkipTalk(game->mainThread->talkingElement);
        }
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
    if (!game || game->isPaused) return;
    
    if (UpdateMenu(game->menu, deltaTicks)) {
        game->inventoryBar->isVisible = false;
        return;
    }
    
    UpdatePlaytime(game->gameState, deltaTicks);
    
    if (game->sequence) {
        UpdateSequence(game->sequence, deltaTicks, game->soundManager);
        if (game->sequence->isFinished) {
            FreeSequence(game->sequence);
            game->sequence = NULL;
        } else {
            return;
        }
    }
    
    UpdateLocation(game->location, deltaTicks);
    UpdateInventoryBar(game->inventoryBar, deltaTicks);
    UpdateIdleProg(game, deltaTicks);
    UpdateThread(game->mainThread, game);
    UpdateFader(&game->fader, deltaTicks);
    
    if (game->fader.state == FaderStateClosed) {
        if (game->actionAfterFadeOut != GameActionNone) {
            DoGameAction(game, game->actionAfterFadeOut);
            game->actionAfterFadeOut = GameActionNone;
        } else if (game->mainThread && !game->mainThread->isActive) {
            FadeIn(&game->fader);
        }
    }
}

void DrawGame(Game *game) {
    if (!game) return;
    
    if (DrawMenu(game->menu)) {
        goto endOfDraw;
    }
    
    if (game->sequence) {
        DrawSequence(game->sequence);
        goto endOfDraw;
    }
    
    DrawLocation(game->location);
    
#ifdef TOUCH
    if (!game->mainThread->isActive && !game->inventoryBar->isVisible && game->inventoryBar->isEnabled && !game->dialog->rootItem) {
        DrawImage(game->inventoryButtonImage, MakeVector(0, SCREEN_HEIGHT - 44));
    }
#endif
    
    DrawLocationOverlays(game->location);
    DrawInventoryBar(game->inventoryBar);
    DrawInventoryItemView(&game->draggingItemView);
    DrawImage(game->focus.image, game->focus.position);
    DrawDialog(game->dialog);
    DrawFader(&game->fader);
    
endOfDraw:
    if (game->isPaused && game->pauseImage) {
        DrawImage(game->pauseImage, MakeVector((SCREEN_WIDTH - game->pauseImage->width) * 0.5, (SCREEN_HEIGHT - game->pauseImage->height) * 0.5));
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
        game->focus.position = MakeVector(fmin(fmax(0, x - width * 0.5), SCREEN_WIDTH - width), fmax(0, y - game->focus.image->height - FocusOffset()));
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
    
    if (game->logFile) {
        fprintf(game->logFile, "\nLocation %s\n", background);
        fflush(game->logFile);
    }
}

void SetGameState(Game *game, GameState *gameState) {
    if (!game || !gameState) return;
    
    if (game->logFile) {
        fprintf(game->logFile, "\n--- New Game State ---\n");
    }
    
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

void SaveGameSlot(Game *game, int slot) {
    char filename[FILE_NAME_SIZE];
    char slotname[SLOT_NAME_SIZE];
    sprintf(filename, "slot_%d", slot);
    SaveGameState(game->gameState, filename, game->config);
#ifdef AUTOSAVE
    GameStateName(game->gameState, slotname, slot == 0);
#else
    GameStateName(game->gameState, slotname, false);
#endif
    SetSlotName(game->slotList, slot, slotname, game->config);
}

void LoadGameSlot(Game *game, int slot) {
    char filename[FILE_NAME_SIZE];
    sprintf(filename, "slot_%d", slot);
    GameState *gameState = LoadGameState(filename, game->config);
    SetGameState(game, gameState);
}

void AutosaveIfPossible(Game *game) {
    if (!game) return;
    // do not save while the main thread is active to avoid broken game states
    if (!game->mainThread->isActive) {
        RefreshGameState(game);
        SaveGameSlot(game, 0);
    }
}

void SafeQuit(Game *game) {
#ifdef AUTOSAVE
    SetShouldQuit();
#else
    if (!game || !game->mainThread || !game->gameState) {
        SetShouldQuit();
        return;
    }
    // is it possible to save the game state?
    if (game->gameState->hasChangedSinceSave && !game->mainThread->isActive && game->fader.state == FaderStateOpen) {
        // ask for quit
        game->isPaused = false;
        game->actionAfterFadeOut = GameActionAskQuit;
        FadeOut(&game->fader);
    } else {
        SetShouldQuit();
    }
#endif
}

void DoGameAction(Game *game, GameAction action) {
    switch (action) {
        case GameActionNone:
            break;
        case GameActionOpenMenu:
            OpenMenu(game->menu, 0);
            break;
        case GameActionAskQuit:
            OpenMenu(game->menu, 5);
            break;
    }
}

void MainPersonDidFinishWalking(Game *game) {
    if (game->selectedId) {
        StartInteraction(game->mainThread, game->selectedId, game->draggedId, game->selectedVerb);
        game->selectedId = 0;
        game->draggedId = 0;
        game->gameState->hasChangedSinceSave = true;
    }
}

void InitLogger(Game *game) {
    if (!game || !game->config) return;
    
    char path[FILENAME_MAX];
    char *prefPath = SDL_GetPrefPath(game->config->organizationName, game->config->gameName);
    sprintf(path, "%s%s", prefPath, "log.txt");
    SDL_free(prefPath);
    
    game->logFile = fopen(path, "a");
    if (!game->logFile) return;
    
    time_t now;
    time(&now);
    fprintf(game->logFile, "\n--- New Session ---\n\n%s", ctime(&now));
}
