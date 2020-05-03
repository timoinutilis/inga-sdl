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

#ifndef Script_h
#define Script_h

#include <stdio.h>

typedef struct Script {
    unsigned char *inga;
    char *itxt;
} Script;

Script *LoadScript(const char *filename);
void FreeScript(Script *script);

unsigned short peekw(Script *script, unsigned long pointer);
unsigned long peekl(Script *script, unsigned long pointer);
const char *peeks(Script *script, unsigned long pointer);

#endif /* Script_h */
