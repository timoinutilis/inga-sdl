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

void SetFocus(Game *game, int x, int y, const char *name);
void UpdateIdleProg(Game *game, int deltaTicks);

Game *CreateGame() {
    Game *game = calloc(1, sizeof(Game));
    if (!game) {
        printf("CreateGame: Out of memory\n");
    } else {
        game->font = LoadFont("Font", 16);
        game->script = LoadScript("story");
        game->gameState = CreateGameState();
        game->inventoryBar = CreateInventoryBar(game->gameState);
        game->dialog = CreateDialog();
        game->mainThread = CreateThread(0);
        game->escImage = LoadImage("Esc", GetGlobalPalette(), true, false);
        
        // Main Person
        Element *element = CreateElement(MainPersonID);
        element->layer = LayerPersons;
        element->position = MakeVector(320, 360);
        element->imageSet = LoadImageSet("Hauptperson", GetGlobalPalette(), true);
        game->mainPerson = element;
    }
    return game;
}

void FreeGame(Game *game) {
    if (!game) return;
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
    
    if (game->sequence) {
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
    if (game->sequence) {
        return;
    }
    
    if (keysym.sym == SDLK_ESCAPE) {
        EscapeThread(game->mainThread);
    }
}

void UpdateGame(Game *game, int deltaTicks) {
    if (!game) return;
    
    if (game->sequence) {
        UpdateSequence(game->sequence, deltaTicks);
        if (game->sequence->isFinished) {
            FreeSequence(game->sequence);
            game->sequence = NULL;
            FadeIn(&game->fader);
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
    if (game->mainPerson && game->idleScript.delay && game->mainPerson->action == ElementActionIdle && !game->mainThread->isActive) {
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
    game->inventoryBar->isEnabled = true;
    game->inventoryBar->isVisible = false;
    FadeIn(&game->fader);
}

void MainPersonDidFinishWalking(Game *game) {
    if (game->selectedId) {
        StartInteraction(game->mainThread, game->selectedId, game->draggedId, game->selectedVerb);
        game->selectedId = 0;
        game->draggedId = 0;
    }
}
