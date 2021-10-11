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

#ifndef Script_h
#define Script_h

#include <stdio.h>
#include "Config.h"

typedef struct Label {
    char name[LABEL_NAME_SIZE];
    unsigned long ptr;
    struct Label *next;
} Label;

typedef struct Script {
    unsigned char *inga;
    char *itxt;
    struct Label *rootLabel;
} Script;

Script *LoadScript(const char *filename);
void FreeScript(Script *script);

unsigned short peekw(Script *script, unsigned long pointer);
unsigned long peekl(Script *script, unsigned long pointer);
const char *peeks(Script *script, unsigned long pointer);

Label *GetLabelWithName(Script *script, const char *name);
Label *GetLabelWithPtr(Script *script, unsigned long ptr);

#endif /* Script_h */
