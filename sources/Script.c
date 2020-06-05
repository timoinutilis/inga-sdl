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
#include <SDL2/SDL.h>
#include "Utils.h"

Script *LoadScript(const char *filename) {
    Script *script = NULL;
    
    char ingaPath[FILENAME_MAX];
    sprintf(ingaPath, "game/Dats/%s.inga", filename);
    
    char itxtPath[FILENAME_MAX];
    sprintf(itxtPath, "game/Dats/%s.itxt", filename);
    
    void *inga = LoadFile(ingaPath, NULL);
    void *itxt = LoadFile(itxtPath, NULL);
    
    if (inga && itxt) {
        script = calloc(1, sizeof(Script));
        if (!script) {
            printf("LoadScript: Out of memory\n");
        } else {
            script->inga = inga;
            script->itxt = itxt;
        }
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
