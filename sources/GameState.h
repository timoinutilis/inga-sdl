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

#ifndef GameState_h
#define GameState_h

#include <stdio.h>
#include <stdbool.h>
#include "Config.h"
#include "Image.h"
#include "GameConfig.h"


typedef struct Variable {
    int id;
    unsigned short value;
    struct Variable *next;
} Variable;

typedef struct InventoryItem {
    int id;
    char name[ELEMENT_NAME_SIZE];
    char filename[FILE_NAME_SIZE];
    Image *image;
    struct InventoryItem *next;
} InventoryItem;

typedef struct GameState {
    Variable *rootVariable;
    int numVariables;
    InventoryItem *rootInventoryItem;
    int numInventoryItems;
    char locationLabel[LABEL_NAME_SIZE];
    Vector startPosition;
    Vector startDirection;
    unsigned long playtimeTicks;
    int textSpeed;
    bool hasChangedSinceSave;
} GameState;

GameState *CreateGameState(void);
void FreeGameState(GameState *gameState);

GameState *LoadGameState(const char *filename, GameConfig *config);
void SaveGameState(GameState *gameState, const char *filename, GameConfig *config);

unsigned short GetVariable(GameState *gameState, int id);
void SetVariable(GameState *gameState, int id, unsigned short value, bool skipIfExists);

bool GetVisibility(GameState *gameState, int locationId, int elementId);
void SetVisibility(GameState *gameState, int locationId, int elementId, bool value, bool skipIfExists);

void AddInventoryItem(GameState *gameState, int id, const char *name, const char *filename, bool atEnd);
void RemoveInventoryItem(GameState *gameState, int id);

void UpdatePlaytime(GameState *gameState, int deltaTicks);
void GameStateName(GameState *gameState, char *name, bool isAutosave);

#endif /* GameState_h */
