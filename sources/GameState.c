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
#include "GameConfig.h"

void FreeVariables(GameState *gameState);
void FreeInventoryItems(GameState *gameState);
void FreeInventoryItem(InventoryItem *item);

GameState *CreateGameState() {
    GameState *gameState = calloc(1, sizeof(GameState));
    if (!gameState) {
        printf("CreateGameState: Out of memory\n");
    } else {
        gameState->startPosition = MakeVector(320, 360);
        gameState->startDirection = MakeVector(0, 1);
        gameState->textSpeed = DEFAULT_TEXT_SPEED;
    }
    return gameState;
}

void FreeGameState(GameState *gameState) {
    if (!gameState) return;
    FreeVariables(gameState);
    FreeInventoryItems(gameState);
    free(gameState);
}

void GameStatePath(GameConfig *config, const char *filename, char *path) {
    char *prefPath = SDL_GetPrefPath(config->organizationName, config->gameName);
    sprintf(path, "%s%s", prefPath, filename);
    SDL_free(prefPath);
}

GameState *LoadGameState(const char *filename, GameConfig *config) {
    GameState *gameState = NULL;
    char path[FILENAME_MAX];
    GameStatePath(config, filename, path);
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    if (!file) {
        printf("LoadGameState: %s\n", SDL_GetError());
    } else {
        gameState = calloc(1, sizeof(GameState));
        if (!gameState) {
            printf("LoadGameState: Out of memory\n");
        } else {
            SDL_RWread(file, gameState->locationLabel, sizeof(char), LABEL_NAME_SIZE);
            SDL_RWread(file, &gameState->startPosition, sizeof(Vector), 1);
            SDL_RWread(file, &gameState->startDirection, sizeof(Vector), 1);
            gameState->playtimeTicks = SDL_ReadBE32(file);
            
            int numVariables = SDL_ReadBE16(file);
            for (int i = 0; i < numVariables; ++i) {
                Uint32 id = SDL_ReadBE32(file);
                Uint16 value = SDL_ReadBE16(file);
                SetVariable(gameState, id, value, false);
            }
            
            int numInventoryItems = SDL_ReadBE16(file);
            for (int i = 0; i < numInventoryItems; ++i) {
                Uint32 id = SDL_ReadBE32(file);
                char itemName[ELEMENT_NAME_SIZE];
                char itemFilename[FILE_NAME_SIZE];
                SDL_RWread(file, itemName, sizeof(char), ELEMENT_NAME_SIZE);
                SDL_RWread(file, itemFilename, sizeof(char), FILE_NAME_SIZE);
                AddInventoryItem(gameState, id, itemName, itemFilename, true);
            }
            
            gameState->textSpeed = SDL_ReadU8(file);
        }
        SDL_RWclose(file);
    }
    return gameState;
}

void SaveGameState(GameState *gameState, const char *filename, GameConfig *config) {
    char path[FILENAME_MAX];
    GameStatePath(config, filename, path);
    SDL_RWops *file = SDL_RWFromFile(path, "wb");
    if (!file) {
        printf("SaveGameState: %s\n", SDL_GetError());
    } else {
        SDL_RWwrite(file, gameState->locationLabel, sizeof(char), LABEL_NAME_SIZE);
        SDL_RWwrite(file, &gameState->startPosition, sizeof(Vector), 1);
        SDL_RWwrite(file, &gameState->startDirection, sizeof(Vector), 1);
        SDL_WriteBE32(file, (Uint32)gameState->playtimeTicks);
        
        SDL_WriteBE16(file, gameState->numVariables);
        Variable *variable = gameState->rootVariable;
        while (variable) {
            SDL_WriteBE32(file, variable->id);
            SDL_WriteBE16(file, variable->value);
            variable = variable->next;
        }
        
        SDL_WriteBE16(file, gameState->numInventoryItems);
        InventoryItem *item = gameState->rootInventoryItem;
        while (item) {
            SDL_WriteBE32(file, item->id);
            SDL_RWwrite(file, item->name, sizeof(char), ELEMENT_NAME_SIZE);
            SDL_RWwrite(file, item->filename, sizeof(char), FILE_NAME_SIZE);
            item = item->next;
        }
        
        SDL_WriteU8(file, gameState->textSpeed);
        
        SDL_RWclose(file);
        
        gameState->hasChangedSinceSave = false;
    }
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
            gameState->numVariables += 1;
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

void AddInventoryItem(GameState *gameState, int id, const char *name, const char *filename, bool atEnd) {
    if (!gameState) return;
    InventoryItem *item = calloc(1, sizeof(InventoryItem));
    if (!item) {
        printf("AddInventoryItem: Out of memory\n");
    } else {
        item->id = id;
        strcpy(item->name, name);
        strcpy(item->filename, filename);
        item->image = LoadImage(filename, GetGlobalPalette(), true, false);
        if (atEnd && gameState->rootInventoryItem) {
            InventoryItem *lastItem = gameState->rootInventoryItem;
            while (lastItem->next) {
                lastItem = lastItem->next;
            }
            lastItem->next = item;
        } else {
            item->next = gameState->rootInventoryItem;
            gameState->rootInventoryItem = item;
        }
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

void UpdatePlaytime(GameState *gameState, int deltaTicks) {
    if (!gameState) return;
    gameState->playtimeTicks += deltaTicks;
}

void GameStateName(GameState *gameState, char *name, bool isAutosave) {
    unsigned long seconds = gameState->playtimeTicks / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    char *title = isAutosave ? "Automatisch" : "Spielzeit";
    if (hours > 0) {
        sprintf(name, "%s %luh %lum %lus", title, hours, minutes % 60, seconds % 60);
    } else {
        sprintf(name, "%s %lum %lus", title, minutes % 60, seconds % 60);
    }
}
