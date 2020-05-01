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

#ifndef Game_h
#define Game_h

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "Location.h"
#include "Font.h"

typedef struct Focus {
    const char *name;
    Image *image;
    Vector position;
} Focus;

typedef struct Game {
    Font *font;
    Location *location;
    Focus focus;
} Game;

Game *CreateGame(void);
void FreeGame(Game *game);
void HandleMouseInGame(Game *game, int x, int y, bool click);
void UpdateGame(Game *game, int deltaTicks);
void DrawGame(Game *game);

#endif /* Game_h */
