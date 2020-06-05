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

#ifndef Sequence_h
#define Sequence_h

#include <stdio.h>
#include "Image.h"

typedef struct Sequence {
    char *text;
    Sint64 textSize;
    Image *image;
    char *currentLine;
    int waitTicks;
    bool isFinished;
} Sequence;

Sequence *LoadSequence(const char *filename);
void FreeSequence(Sequence *sequence);
void UpdateSequence(Sequence *sequence, int deltaTicks);
void DrawSequence(Sequence *sequence);

#endif /* Sequence_h */
