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

#include "Script.h"
#include "SDL_includes.h"
#include "Utils.h"

void ParseLog(Script *script, void *log);
void FreeLabels(Script *script);

Script *LoadScript(const char *filename) {
    Script *script = NULL;
    
    char ingaPath[FILENAME_MAX];
    GameFilePath(ingaPath, "Dats", filename, "inga");
    
    char itxtPath[FILENAME_MAX];
    GameFilePath(itxtPath, "Dats", filename, "itxt");
    
    char logPath[FILENAME_MAX];
    GameFilePath(logPath, "Dats", "SkriptShellLog", NULL);
    
    void *inga = LoadFile(ingaPath, NULL);
    void *itxt = LoadFile(itxtPath, NULL);
    void *log = LoadFile(logPath, NULL);
    
    if (inga && itxt && log) {
        script = calloc(1, sizeof(Script));
        if (!script) {
            printf("LoadScript: Out of memory\n");
        } else {
            script->inga = inga;
            script->itxt = itxt;
            ParseLog(script, log);
        }
    }
    
    if (log) {
        free(log);
    }
    
    if (!script) {
        if (inga) {
            free(inga);
        }
        if (itxt) {
            free(itxt);
        }
    }
    
    return script;
}

void FreeScript(Script *script) {
    if (!script) return;
    free(script->inga);
    free(script->itxt);
    FreeLabels(script);
    free(script);
}

// word
unsigned short peekw(Script *script, unsigned long pointer) {
    int byte1 = script->inga[pointer + 1];
    int byte2 = script->inga[pointer];
    return (byte2 << 8) | byte1;
}

// long
unsigned long peekl(Script *script, unsigned long pointer) {
    int byte1 = script->inga[pointer + 3];
    int byte2 = script->inga[pointer + 2];
    int byte3 = script->inga[pointer + 1];
    int byte4 = script->inga[pointer];
    return (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1;
}

// string
const char *peeks(Script *script, unsigned long pointer) {
    unsigned long stringPointer = peekl(script, pointer);
    return &script->itxt[stringPointer];
}

void ParseLog(Script *script, void *log) {
    char *lineStart = log;
    char number[10];
    for (char *c = log; *c != 0; ++c) {
        if (*c == 0x0A) {
            Label *label = calloc(sizeof(Label), 1);
            
            if (!label) {
                printf("ParseLog: Out of memory\n");
                return;
            } else {
                char *space = strchr(lineStart, ' ');
                memcpy(label->name, lineStart, space - lineStart);
                memset(number, 0, 10);
                memcpy(number, space + 4, c - space - 4);
                label->ptr = atol(number);
                
                label->next = script->rootLabel;
                script->rootLabel = label;
            }
            lineStart = c + 1;
        }
    }
}

void FreeLabels(Script *script) {
    if (!script) return;
    Label *label = script->rootLabel;
    while (label) {
        Label *next = label->next;
        free(label);
        label = next;
    }
    script->rootLabel = NULL;
}

Label *GetLabelWithName(Script *script, const char *name) {
    if (!script) return NULL;
    if (name[0] == 0) return NULL;
    Label *label = script->rootLabel;
    while (label) {
        if (strcmp(label->name, name) == 0) {
            return label;
        }
        label = label->next;
    }
    printf("GetLabelWithName: Not found (%s)\n", name);
    return NULL;
}

Label *GetLabelWithPtr(Script *script, unsigned long ptr) {
    if (!script) return NULL;
    Label *label = script->rootLabel;
    while (label) {
        if (label->ptr == ptr) {
            return label;
        }
        label = label->next;
    }
    printf("GetLabelWithPtr: Not found (%lu)\n", ptr);
    return NULL;
}
