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

#include "GameState.h"
#include <stdlib.h>
#include <string.h>
#include "Global.h"

void FreeVariables(GameState *gameState);
void FreeInventoryItems(GameState *gameState);
void FreeInventoryItem(InventoryItem *item);

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
    FreeInventoryItems(gameState);
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

void AddInventoryItem(GameState *gameState, int id, const char *name, const char *filename) {
    if (!gameState) return;
    InventoryItem *item = calloc(1, sizeof(InventoryItem));
    if (!item) {
        printf("AddInventoryItem: Out of memory\n");
    } else {
        item->id = id;
        strcpy(item->name, name);
        strcpy(item->filename, filename);
        item->image = LoadImage(filename, GetGlobalPalette(), true, false);
        item->next = gameState->rootInventoryItem;
        gameState->rootInventoryItem = item;
        gameState->numInventoryItems += 1;
    }
}

void RemoveInventoryItem(GameState *gameState, int id) {
    if (!gameState || !gameState->rootInventoryItem) return;
    if (gameState->rootInventoryItem->id == id) {
        InventoryItem *item = gameState->rootInventoryItem;
        gameState->rootInventoryItem = gameState->rootInventoryItem->next;
        FreeInventoryItem(item);
        gameState->numInventoryItems -= 1;
    } else {
        InventoryItem *currItem = gameState->rootInventoryItem->next;
        InventoryItem *prevItem = gameState->rootInventoryItem;
        while (currItem) {
            if (currItem->id == id) {
                prevItem->next = currItem->next;
                FreeInventoryItem(currItem);
                gameState->numInventoryItems -= 1;
                break;
            }
            prevItem = currItem;
            currItem = currItem->next;
        };
    }
}

void FreeInventoryItems(GameState *gameState) {
    if (!gameState) return;
    InventoryItem *item = gameState->rootInventoryItem;
    while (item) {
        InventoryItem *next = item->next;
        FreeInventoryItem(item);
        item = next;
    }
    gameState->rootInventoryItem = NULL;
}

void FreeInventoryItem(InventoryItem *item) {
    if (!item) return;
    FreeImage(item->image);
    free(item);
}
