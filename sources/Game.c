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

void SetFocus(Game *game, int x, int y, const char *name);
void UpdateIdleProg(Game *game, int deltaTicks);
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
        game->gameState = CreateGameState();
        game->inventoryBar = CreateInventoryBar(game->gameState);
        game->dialog = CreateDialog();
        game->mainThread = CreateThread(0);
        game->escImage = LoadImage("Esc", GetGlobalPalette(), true, false);
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
        
        HideCursor();
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

void HandleMouseInGame(Game *game, int x, int y, ButtonState buttonState) {
    if (!game) return;
    
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
    
    game->draggingItemView.position = MakeVector(x, y);
    
    if (HandleMouseInInventoryBar(game->inventoryBar, x, y, buttonState)) {
        if (game->inventoryBar->focusedButton != InventoryBarButtonNone) {
            SetFocus(game, x, y, NULL);
            if (buttonState == ButtonStateClickLeft) {
                if (game->inventoryBar->focusedButton == InventoryBarButtonMenu) {
                    RefreshGameState(game);
                    game->openMenuAfterFadeOut = true;
                    FadeOut(&game->fader);
                }
            }
            return;
        }
        InventoryItem *focusedItem = GetItemInInventoryBarAt(game->inventoryBar, x, y);
        if (focusedItem) {
            SetFocus(game, x, y, focusedItem->name);
            if (buttonState == ButtonStateClickLeft) {
                if (game->draggingItemView.item) {
                    if (focusedItem->id == game->draggingItemView.item->id) {
                        if (game->logFile) {
                            fprintf(game->logFile, "Look at %s\n", focusedItem->name);
                        }
                        StartInteraction(game->mainThread, focusedItem->id, 0, VerbLook);
                    } else {
                        if (game->logFile) {
                            fprintf(game->logFile, "Use %s with %s\n", focusedItem->name, game->draggingItemView.item->name);
                        }
                        StartInteraction(game->mainThread, focusedItem->id, game->draggingItemView.item->id, VerbUse);
                        game->inventoryBar->isVisible = false;
                    }
                    game->draggingItemView.item = NULL;
                } else {
                    SetCursor(game->cursorDrag);
                    game->draggingItemView.item = focusedItem;
                }
            } else if (buttonState == ButtonStateClickRight) {
                game->draggingItemView.item = NULL;
                if (game->logFile) {
                    fprintf(game->logFile, "Look at %s\n", focusedItem->name);
                }
                StartInteraction(game->mainThread, focusedItem->id, 0, VerbLook);
            }
        } else {
            SetFocus(game, x, y, NULL);
        }
        return;
    }
    
    if (!game->location) return;
    
    Element *focusedElement = canHover(buttonState) ? GetElementAt(game->location, x, y) : NULL;
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
        Element *person = GetElement(game->location, MainPersonID);
        ElementMoveTo(person, x, y, 0, false);
    }
}

void HandleKeyInGame(Game *game, SDL_Keysym keysym) {
    if (!game) return;
    
    if (HandleKeyInSequence(game->sequence, keysym)) {
        return;
    }
    
    if (game->fader.state != FaderStateOpen) {
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
    
    UpdateLocation(game->location, deltaTicks);
    UpdateInventoryBar(game->inventoryBar, deltaTicks);
    UpdateIdleProg(game, deltaTicks);
    UpdateThread(game->mainThread, game);
    UpdateFader(&game->fader, deltaTicks);
    
    if (game->fader.state == FaderStateClosed) {
        if (game->openMenuAfterFadeOut) {
            game->openMenuAfterFadeOut = false;
            OpenMenu(game->menu);
        } else if (game->mainThread && !game->mainThread->isActive) {
            FadeIn(&game->fader);
        }
    }
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
    
    if (game->mainThread->escptr) {
        DrawImage(game->escImage, MakeVector(1, 1));
    }
    
    DrawLocationOverlays(game->location);
    DrawInventoryBar(game->inventoryBar);
    DrawImage(game->focus.image, game->focus.position);
    DrawInventoryItemView(&game->draggingItemView);
    DrawDialog(game->dialog);
    DrawFader(&game->fader);
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
        game->focus.position = MakeVector(fmin(fmax(0, x - width * 0.5), SCREEN_WIDTH - width), y - game->focus.image->height - FocusOffset());
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

void MainPersonDidFinishWalking(Game *game) {
    if (game->selectedId) {
        StartInteraction(game->mainThread, game->selectedId, game->draggedId, game->selectedVerb);
        game->selectedId = 0;
        game->draggedId = 0;
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
