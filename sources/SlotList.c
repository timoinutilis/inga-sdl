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

#include "SlotList.h"
#include "SDL_includes.h"
#include "GameConfig.h"

void SaveSlotList(SlotList *list, GameConfig *config);
void SlotPath(GameConfig *config, char *path);


SlotList *CreateSlotList(GameConfig *config) {
    SlotList *list = calloc(1, sizeof(SlotList));
    if (!list) {
        printf("CreateSlotList: Out of memory\n");
    } else {
        for (int i = 0; i < NUM_SAVE_SLOTS; ++i) {
            sprintf(list->slotNames[i], "%d - Leer", i + 1);
        }
        char path[FILENAME_MAX];
        SlotPath(config, path);
        SDL_RWops *file = SDL_RWFromFile(path, "rb");
        if (!file) {
            printf("CreateSlotList: %s\n", SDL_GetError());
        } else {
            SDL_RWread(file, list, sizeof(SlotList), 1);
            SDL_RWclose(file);
        }
    }
    return list;
}

void FreeSlotList(SlotList *list) {
    if (!list) return;
    free(list);
}

void SetSlotName(SlotList *list, int slot, const char *name, GameConfig *config) {
    if (!list) return;
    sprintf(list->slotNames[slot], "%d - %s", slot + 1, name);
    SaveSlotList(list, config);
}

void SaveSlotList(SlotList *list, GameConfig *config) {
    if (!list) return;
    char path[FILENAME_MAX];
    SlotPath(config, path);
    SDL_RWops *file = SDL_RWFromFile(path, "wb");
    if (!file) {
        printf("SaveSlotList: %s\n", SDL_GetError());
    } else {
        SDL_RWwrite(file, list, sizeof(SlotList), 1);
        SDL_RWclose(file);
    }
}

void SlotPath(GameConfig *config, char *path) {
    char *prefPath = SDL_GetPrefPath(config->organizationName, config->gameName);
    sprintf(path, "%sslotlist", prefPath);
    SDL_free(prefPath);
}
