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

#include "GameConfig.h"
#include <stdlib.h>
#include "Utils.h"

GameConfig *LoadGameConfig(void) {
    GameConfig *config = NULL;
    
    char path[FILENAME_MAX];
    GameFilePath(path, NULL, "config", "json");
    
    char *jsonString = LoadFile(path, NULL);
    if (jsonString) {
        cJSON *json = cJSON_Parse(jsonString);
        if (!json) {
            printf("LoadGameConfig: json parse error\n");
        } else {
            config = calloc(1, sizeof(GameConfig));
            if (!config) {
                printf("LoadGameConfig: Out of memory\n");
            } else {
                config->json = json;
                
                const cJSON *organizationName = cJSON_GetObjectItemCaseSensitive(json, "organizationName");
                if (cJSON_IsString(organizationName)) {
                    config->organizationName = organizationName->valuestring;
                }
                
                const cJSON *gameName = cJSON_GetObjectItemCaseSensitive(json, "gameName");
                if (cJSON_IsString(gameName)) {
                    config->gameName = gameName->valuestring;
                }
                
                const cJSON *paletteFilename = cJSON_GetObjectItemCaseSensitive(json, "paletteFilename");
                if (cJSON_IsString(paletteFilename)) {
                    config->paletteFilename = paletteFilename->valuestring;
                }
                
                const cJSON *version = cJSON_GetObjectItemCaseSensitive(json, "version");
                if (cJSON_IsString(version)) {
                    config->version = version->valuestring;
                }
                
                const cJSON *languageNames = cJSON_GetObjectItemCaseSensitive(json, "languageNames");
                const cJSON *languageCodes = cJSON_GetObjectItemCaseSensitive(json, "languageCodes");
                if (cJSON_IsArray(languageNames) && cJSON_IsArray(languageCodes)) {
                    int num = cJSON_GetArraySize(languageNames);
                    int numCodes = cJSON_GetArraySize(languageCodes);
                    if (num > 0 && numCodes == num) {
                        config->languageNames = calloc(num, sizeof(char *));
                        config->languageCodes = calloc(num, sizeof(char *));
                        if (!config->languageNames || !config->languageCodes) {
                            printf("LoadGameConfig: Out of memory\n");
                            FreeGameConfig(config);
                            config = NULL;
                        } else {
                            config->numLanguages = num;
                            for (int i = 0; i < num; i++) {
                                config->languageNames[i] = cJSON_GetArrayItem(languageNames, i)->valuestring;
                                config->languageCodes[i] = cJSON_GetArrayItem(languageCodes, i)->valuestring;
                            }
                        }
                    }
                }
                                
                if (!organizationName || !gameName || !paletteFilename || !version) {
                    printf("LoadGameConfig: missing values\n");
                    FreeGameConfig(config);
                    config = NULL;
                }
            }
        }
        free(jsonString);
    }
    return config;
}

void FreeGameConfig(GameConfig *config) {
    if (!config) return;
    if (config->languageNames) {
        free(config->languageNames);
    }
    if (config->languageCodes) {
        free(config->languageCodes);
    }
    cJSON_Delete(config->json);
    free(config);
}
