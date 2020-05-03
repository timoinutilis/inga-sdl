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
        game->script = LoadScript("story");
        game->mainThread = CreateThread(0);
    }
    return game;
}

void FreeGame(Game *game) {
    if (!game) return;
    FreeImage(game->focus.image);
    FreeLocation(game->location);
    FreeThread(game->mainThread);
    FreeScript(game->script);
    FreeFont(game->font);
    free(game);
}

void HandleMouseInGame(Game *game, int x, int y, bool click) {
    if (!game) return;
    if (game->mainThread->isActive) {
        SetFocus(game, x, y, NULL);
    } else {
        HandleMouseInLocation(game->location, x, y, click);
        if (game->location) {
            SetFocus(game, x, y, game->location->currentFocusName);
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
        if (name) {
            SDL_Color color = {255, 255, 0, 255};
            game->focus.image = CreateImageFromText(name, game->font, color);
        }
    }
    if (game->focus.image) {
        int width = game->focus.image->width;
        game->focus.position = MakeVector(fmin(fmax(0, x - width * 0.5), SCREEN_WIDTH - width), y - game->focus.image->height - 4);
    }
}
