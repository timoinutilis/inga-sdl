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

#include "Game.h"
#include "Global.h"

void SetFocus(Game *game, int x, int y, const char *name);

Game *CreateGame() {
    Game *game = calloc(1, sizeof(Game));
    if (!game) {
        printf("CreateGame: Out of memory\n");
    } else {
        game->font = LoadFont("Orbitron-Medium", 16);
        game->script = LoadScript("story");
        game->gameState = CreateGameState();
        game->inventoryBar = CreateInventoryBar(game->gameState);
        game->mainThread = CreateThread(0);
        
        // Main Person
        Element *element = CreateElement(MainPersonID);
        element->position = MakeVector(320, 360);
        element->imageSet = LoadImageSet("Hauptperson", GetGlobalPalette(), true);
        game->mainPerson = element;
    }
    return game;
}

void FreeGame(Game *game) {
    if (!game) return;
    FreeImage(game->focus.image);
    FreeLocation(game->location);
    FreeElement(game->mainPerson);
    FreeThread(game->mainThread);
    FreeInventoryBar(game->inventoryBar);
    FreeGameState(game->gameState);
    FreeScript(game->script);
    FreeFont(game->font);
    free(game);
}

void HandleMouseInGame(Game *game, int x, int y, int buttonIndex) {
    if (!game) return;
    if (game->mainThread && game->mainThread->isActive) {
        SetFocus(game, x, y, NULL);
        game->draggingItemView.item = NULL;
    } else {
        game->draggingItemView.position = MakeVector(x, y);
        if (HandleMouseInInventoryBar(game->inventoryBar, x, y, buttonIndex)) {
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
                Element *person = GetElement(game->location, MainPersonID);
                if (!person->isVisible) {
                    MainPersonDidFinishWalking(game);
                } else {
                    Vector target = GetElementTarget(focusedElement, person->position);
                    ElementMoveTo(person, target.x, target.y, 0, false);
                }
            }
        } else {
            SetFocus(game, x, y, NULL);
            if (buttonIndex == SDL_BUTTON_LEFT) {
                game->selectedId = 0;
                Element *person = GetElement(game->location, MainPersonID);
                ElementMoveTo(person, x, y, 0, false);
            }
        }
    }
}

void HandleKeyInGame(Game *game, SDL_Keysym keysym) {
    if (keysym.sym == SDLK_ESCAPE) {
        EscapeThread(game->mainThread);
    }
}

void UpdateGame(Game *game, int deltaTicks) {
    if (!game) return;
    UpdateThread(game->mainThread, game);
    UpdateLocation(game->location, deltaTicks);
    UpdateInventoryBar(game->inventoryBar, deltaTicks);
}

void DrawGame(Game *game) {
    if (!game) return;
    DrawLocation(game->location);
    DrawInventoryBar(game->inventoryBar);
    DrawImage(game->focus.image, game->focus.position);
    DrawInventoryItemView(&game->draggingItemView);
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

void SetLocation(Game *game, int id, const char *background) {
    game->mainThread->talkingElement = NULL;
    ElementStop(game->mainPerson);
    game->mainPerson->isVisible = true;
    ResetDefaultAnimations(game->mainPerson);
    FreeLocation(game->location);
    game->location = CreateLocation(id, background);
    game->location->game = game;
    AddElement(game->location, game->mainPerson);
}

void MainPersonDidFinishWalking(Game *game) {
    if (game->selectedId) {
        StartInteraction(game->mainThread, game->selectedId, game->draggedId, game->selectedVerb);
        game->selectedId = 0;
        game->draggedId = 0;
    }
}
