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
// along with LowRes NX.  If not, see <http://www.gnu.org/licenses/>.
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
        game->mainThread = CreateThread(0);
        game->paletteImage = LoadImage("Thronsaal", NULL, false, true);
        
        // Main Person
        Element *element = CreateElement(MainPersonID);
        element->position = MakeVector(320, 360);
        element->imageSet = LoadImageSet("Hauptperson", game->paletteImage->surface->format->palette, true);
        game->mainPerson = element;
    }
    return game;
}

void FreeGame(Game *game) {
    if (!game) return;
    FreeImage(game->focus.image);
    FreeLocation(game->location);
    FreeElement(game->mainPerson);
    FreeImage(game->paletteImage);
    FreeThread(game->mainThread);
    FreeGameState(game->gameState);
    FreeScript(game->script);
    FreeFont(game->font);
    free(game);
}

void HandleMouseInGame(Game *game, int x, int y, int buttonIndex) {
    if (!game) return;
    if (game->mainThread && game->mainThread->isActive) {
        SetFocus(game, x, y, NULL);
    } else {
        if (!game->location) return;
        Element *focusedElement = GetElementAt(game->location, x, y);
        if (focusedElement) {
            SetFocus(game, x, y, focusedElement->name);
            if (buttonIndex > 0) {
                game->selectedId = focusedElement->id;
                game->selectedVerb = buttonIndex == SDL_BUTTON_RIGHT ? VerbLook : VerbUse;
                Element *person = GetElement(game->location, MainPersonID);
                if (!person->isVisible) {
                    MainPersonDidFinishWalking(game);
                } else if (focusedElement->target.y) {
                    ElementMoveTo(person, focusedElement->target.x, focusedElement->target.y, 2);
                } else {
                    ElementMoveTo(person, focusedElement->position.x, focusedElement->position.y, 2);
                }
            }
        } else {
            SetFocus(game, x, y, NULL);
            if (buttonIndex == SDL_BUTTON_LEFT) {
                game->selectedId = 0;
                Element *person = GetElement(game->location, MainPersonID);
                ElementMoveTo(person, x, y, 2);
            }
        }
    }
}

void HandleKeyInGame(Game *game, SDL_Keysym keysym) {
    if (keysym.sym == SDLK_ESCAPE) {
        if (game->mainThread && game->mainThread->escptr) {
            game->mainThread->ptr = game->mainThread->escptr;
            game->mainThread->escptr = 0;
        }
    }
}

void UpdateGame(Game *game, int deltaTicks) {
    if (!game) return;
    UpdateThread(game->mainThread, game);
    UpdateLocation(game->location, deltaTicks);
}

void DrawGame(Game *game) {
    if (!game) return;
    DrawLocation(game->location);
    DrawImage(game->focus.image, game->focus.position);
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
    FreeLocation(game->location);
    game->location = CreateLocation(id, background);
    game->location->game = game;
    AddElement(game->location, game->mainPerson);
}

void MainPersonDidFinishWalking(Game *game) {
    if (game->selectedId) {
        StartInteraction(game->mainThread, game->selectedId, game->selectedVerb);
        game->selectedId = 0;
    }
}
