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

#include "Sequence.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include "Utils.h"
#include "Config.h"

Sequence *LoadSequence(const char *filename) {
    Sequence *sequence = NULL;
    
    char path[FILENAME_MAX];
    sprintf(path, "game/Dats/%s.isq", filename);
    
    Sint64 size = 0;
    char *text = LoadFile(path, &size);
    
    if (text) {
        sequence = calloc(1, sizeof(Sequence));
        if (!sequence) {
            printf("LoadSequence: Out of memory\n");
            free(text);
        } else {
            // replace new lines with end of string
            for (char *c = text; c < text + size; ++c) {
                if (*c == 0x0A) *c = 0;
            }
            
            sequence->text = text;
            sequence->textSize = size;
            sequence->currentLine = text;
        }
    }
        
    return sequence;
}

void FreeSequence(Sequence *sequence) {
    if (!sequence) return;
    FreeImage(sequence->image);
    free(sequence->text);
    free(sequence);
}

void UpdateSequence(Sequence *sequence, int deltaTicks) {
    if (!sequence || sequence->isFinished) return;
    
    if (sequence->waitTicks > 0) {
        sequence->waitTicks -= deltaTicks;
        if (sequence->waitTicks < 0) sequence->waitTicks = 0;
        return;
    }
    
    if (sequence->currentLine >= sequence->text + sequence->textSize) {
        sequence->isFinished = true;
        return;
    }
    
    if (sequence->currentLine[0] == '$') {
        char command = sequence->currentLine[1];
        if (command == ':') {
            // load image
            char *filename = &sequence->currentLine[2];
            FreeImage(sequence->image);
            sequence->image = LoadImage(filename, NULL, false, false);
        } else if (command == 'W') {
            // wait
            char *number = &sequence->currentLine[2];
            int seconds = atoi(number);
            sequence->waitTicks = seconds * 1000;
        }
    } else {
        // text to show (not implemented)
    }
    
    // next line
    sequence->currentLine = sequence->currentLine + strlen(sequence->currentLine) + 1;
}

void DrawSequence(Sequence *sequence) {
    if (!sequence) return;
    DrawImage(sequence->image, MakeVector(0, 0));
}
