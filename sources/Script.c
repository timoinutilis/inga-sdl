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
