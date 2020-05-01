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

Game *CreateGame(SDL_Renderer *renderer) {
    Game *game = calloc(1, sizeof(Game));
    if (!game) {
        printf("CreateGame: Out of memory\n");
    } else {
        game->font = LoadFont("Orbitron-Medium", 16);
                
        Location *location = CreateLocation(1, "HafenL", renderer);
        location->game = game;
        SDL_Palette *palette = location->image->surface->format->palette;
        
        Element *person = CreateElement(0);
        person->imageSet = LoadImageSetIPE("Hauptperson", renderer, palette, true);
        person->position = MakeVector(360, 380);
        AddElement(location, person);
        
        Element *deco1 = CreateElement(1);
        deco1->image = LoadImageIBM("HafenWasserL", renderer, palette, false, false);
        deco1->position = MakeVector(210, 440);
        AddElement(location, deco1);
        
        Element *deco2 = CreateElement(2);
        deco2->image = LoadImageIBM("HafenWasserLa", renderer, palette, false, false);
        deco2->position = MakeVector(0, 450);
        AddElement(location, deco2);
        
        Element *deco3 = CreateElement(3);
        deco3->image = CreateImageFromText("Hello Text!", game->font, renderer);
        deco3->position = MakeVector(320 - deco3->image->width / 2, 240 - deco3->image->height / 2);
        AddElement(location, deco3);
        
        game->mainPerson = person;
        game->location = location;
    }
    return game;
}

void FreeGame(Game *game) {
    if (!game) return;
    FreeLocation(game->location);
    FreeFont(game->font);
    free(game);
}

void UpdateGame(Game *game, int deltaTicks) {
    if (!game) return;
    UpdateLocation(game->location, deltaTicks);
}

void DrawGame(Game *game, SDL_Renderer *renderer) {
    if (!game) return;
    DrawLocation(game->location, renderer);
}
