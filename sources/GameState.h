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

#ifndef GameState_h
#define GameState_h

#include <stdio.h>
#include <stdbool.h>

typedef struct Variable {
    int id;
    unsigned short value;
    struct Variable *next;
} Variable;

typedef struct GameState {
    Variable *rootVariable;
} GameState;

GameState *CreateGameState(void);
void FreeGameState(GameState *gameState);

unsigned short GetVariable(GameState *gameState, int id);
void SetVariable(GameState *gameState, int id, unsigned short value, bool skipIfExists);

bool GetVisibility(GameState *gameState, int locationId, int elementId);
void SetVisibility(GameState *gameState, int locationId, int elementId, bool value, bool skipIfExists);

#endif /* GameState_h */
