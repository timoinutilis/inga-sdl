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

void SetFocus(Game *game, int x, int y, const char *name);

Game *CreateGame() {
    Game *game = calloc(1, sizeof(Game));
    if (!game) {
        printf("CreateGame: Out of memory\n");
    } else {
        game->font = LoadFont("Orbitron-Medium", 16);
                
        Location *location = CreateLocation(1, "HafenL");
        location->game = game;
        SDL_Palette *palette = location->image->surface->format->palette;
        
        Element *person = CreateElement(0);
        person->imageSet = LoadImageSet("Hauptperson", palette, true);
        person->position = MakeVector(360, 380);
        person->talkFont = game->font;
        AddElement(location, person);
        
        Element *deco1 = CreateElement(1);
        deco1->image = LoadImage("HafenWasserL", palette, false, false);
        deco1->position = MakeVector(210, 440);
        AddElement(location, deco1);
        
        Element *deco2 = CreateElement(2);
        deco2->image = LoadImage("HafenWasserLa", palette, false, false);
        deco2->position = MakeVector(0, 450);
        AddElement(location, deco2);
        
        Element *field1 = CreateElement(3);
        field1->isSelectable = true;
        field1->selectionRect = MakeRectFromTo(252, 178, 444, 335);
        field1->target = MakeVector(351, 341);
        strcpy(field1->name, "zur Übersicht");
        AddElement(location, field1);
        
        Element *field2 = CreateElement(4);
        field2->isSelectable = true;
        field2->selectionRect = MakeRectFromTo(209, 172, 236, 365);
        field2->target = MakeVector(255, 365);
        strcpy(field2->name, "Eingang");
        AddElement(location, field2);

        Element *field3 = CreateElement(5);
        field3->isSelectable = true;
        field3->selectionRect = MakeRectFromTo(49, 411, 244, 479);
        field3->target = MakeVector(142, 428);
        strcpy(field3->name, "zum Schiff");
        AddElement(location, field3);

        Element *field4 = CreateElement(6);
        field4->isSelectable = true;
        field4->selectionRect = MakeRectFromTo(598, 194, 639, 399);
        field4->target = MakeVector(639, 375);
        strcpy(field4->name, "zu Dock 5");
        AddElement(location, field4);

        Element *field5 = CreateElement(7);
        field5->isSelectable = true;
        field5->selectionRect = MakeRectFromTo(0, 280, 177, 380);
        field5->target = MakeVector(129, 398);
        strcpy(field5->name, "Fässer");
        AddElement(location, field5);
        
        game->location = location;
    }
    return game;
}

void FreeGame(Game *game) {
    if (!game) return;
    FreeImage(game->focus.image);
    FreeLocation(game->location);
    FreeFont(game->font);
    free(game);
}

void HandleMouseInGame(Game *game, int x, int y, bool click) {
    if (!game) return;
    HandleMouseInLocation(game->location, x, y, click);
    if (game->location) {
        SetFocus(game, x, y, game->location->currentFocusName);
    }
}

void UpdateGame(Game *game, int deltaTicks) {
    if (!game) return;
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
        if (name) {
            game->focus.image = CreateImageFromText(name, game->font);
        }
    }
    if (game->focus.image) {
        int width = game->focus.image->width;
        game->focus.position = MakeVector(fmin(fmax(0, x - width * 0.5), SCREEN_WIDTH - width), y - game->focus.image->height - 4);
    }
}
