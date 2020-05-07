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

#include "GameState.h"
#include <stdlib.h>

void FreeVariables(GameState *gameState);

GameState *CreateGameState(void) {
    GameState *gameState = calloc(1, sizeof(GameState));
    if (!gameState) {
        printf("CreateGameState: Out of memory\n");
    } else {
    }
    return gameState;
}

void FreeGameState(GameState *gameState) {
    if (!gameState) return;
    FreeVariables(gameState);
    free(gameState);
}

Variable *GetVariableObject(GameState *gameState, int id) {
    if (!gameState) return NULL;
    Variable *variable = gameState->rootVariable;
    while (variable) {
        if (variable->id == id) {
            return variable;
        }
        variable = variable->next;
    }
    return NULL;
}

unsigned short GetVariable(GameState *gameState, int id) {
    Variable *variable = GetVariableObject(gameState, id);
    if (variable) {
        return variable->value;
    }
    return 0;
}

void SetVariable(GameState *gameState, int id, unsigned short value, bool skipIfExists) {
    if (!gameState) return;
    Variable *variable = GetVariableObject(gameState, id);
    if (variable) {
        if (!skipIfExists) {
            variable->value = value;
        }
    } else {
        variable = calloc(1, sizeof(Variable));
        if (!variable) {
            printf("SetVariable: Out of memory\n");
        } else {
            variable->id = id;
            variable->value = value;
            variable->next = gameState->rootVariable;
            gameState->rootVariable = variable;
        }
    }
}

void FreeVariables(GameState *gameState) {
    if (!gameState) return;
    Variable *variable = gameState->rootVariable;
    while (variable) {
        Variable *next = variable->next;
        free(variable);
        variable = next;
    }
    gameState->rootVariable = NULL;
}

bool GetVisibility(GameState *gameState, int locationId, int elementId) {
    int id = (locationId << 16) + elementId;
    Variable *variable = GetVariableObject(gameState, id);
    if (variable) {
        return variable->value == 1;
    }
    return true;
}

void SetVisibility(GameState *gameState, int locationId, int elementId, bool value, bool skipIfExists) {
    int id = (locationId << 16) + elementId;
    SetVariable(gameState, id, value ? 1 : 0, skipIfExists);
}
