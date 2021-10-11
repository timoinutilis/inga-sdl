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
#include "SDL_includes.h"
#include <stdlib.h>
#include "Utils.h"
#include "Config.h"
#include "Global.h"

Sequence *LoadSequence(const char *filename) {
    Sequence *sequence = NULL;
    
    char path[FILENAME_MAX];
    GameFilePath(path, "Dats", filename, "isq");
    
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
            InitFader(&sequence->fader, SEQUENCE_FADE_DURATION);
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

bool HandleMouseInSequence(Sequence *sequence, int x, int y, ButtonState buttonState) {
    if (!sequence || sequence->isFinished) return false;
    if (buttonState == SelectionButtonState()) {
        sequence->isWaitingForClick = false;
    }
    return true;
}

bool HandleKeyInSequence(Sequence *sequence, SDL_Keysym keysym) {
    if (!sequence || sequence->isFinished) return false;
    if (keysym.sym == SDLK_ESCAPE) {
        sequence->waitTicks = 0;
        sequence->isWaitingForClick = false;
        sequence->wasSkipped = true;
    }
    return true;
}

void UpdateSequence(Sequence *sequence, int deltaTicks, SoundManager *soundManager) {
    if (!sequence || sequence->isFinished) return;
    
    UpdateFader(&sequence->fader, deltaTicks);
    
    if (sequence->waitTicks > 0) {
        sequence->waitTicks -= deltaTicks;
        if (sequence->waitTicks < 0) sequence->waitTicks = 0;
        return;
    }
    
    if (sequence->isWaitingForClick) {
        return;
    }
    
    if (sequence->wasSkipped || sequence->currentLine >= sequence->text + sequence->textSize) {
        // end of sequence
        if (sequence->fader.state != FaderStateClosed) {
            if (sequence->fader.state != FaderStateFadingOut) {
                FadeOut(&sequence->fader);
            }
        } else {
            sequence->isFinished = true;
        }
        return;
    }
    
    if (sequence->currentLine[0] == '$') {
        char command = sequence->currentLine[1];
        if (command == ':') {
            // load image
            if (sequence->fader.state != FaderStateClosed) {
                if (sequence->fader.state != FaderStateFadingOut) {
                    FadeOut(&sequence->fader);
                }
                return;
            }
            char *filename = &sequence->currentLine[2];
            FreeImage(sequence->image);
            sequence->image = LoadImage(filename, NULL, false, false);
            FadeIn(&sequence->fader);
        } else if (command == 'W') {
            // wait
            char *number = &sequence->currentLine[2];
            int seconds = atoi(number);
            sequence->waitTicks = seconds * 1000;
        } else if (command == 'M') {
            // wait for click
            sequence->isWaitingForClick = true;
        } else if (command == 'S') {
            // stop music
            StopTrack(soundManager);
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
    DrawFader(&sequence->fader);
}
