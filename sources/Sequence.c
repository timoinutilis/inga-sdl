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
    if (sequence->image) {
        Vector pos;
        pos.x = floorf((SCREEN_WIDTH - sequence->image->width) * 0.5f);
        pos.y = floorf((SCREEN_HEIGHT - sequence->image->height) * 0.5f);
        DrawImage(sequence->image, pos);
    }
}
