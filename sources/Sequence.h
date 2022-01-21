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

#ifndef Sequence_h
#define Sequence_h

#include <stdio.h>
#include <stdbool.h>
#include "Image.h"
#include "Fader.h"
#include "Enums.h"
#include "SoundManager.h"
#include "Font.h"

typedef struct Sequence {
    char *text;
    Sint64 textSize;
    Image *image;
    char *currentLine;
    int waitTicks;
    bool isWaitingForClick;
    bool wasSkipped;
    bool isFinished;
    Fader fader;
    Font *font;
} Sequence;

Sequence *LoadSequence(const char *filename, Font *font);
void FreeSequence(Sequence *sequence);
bool HandleMouseInSequence(Sequence *sequence, int x, int y, ButtonState buttonState);
bool HandleKeyInSequence(Sequence *sequence, SDL_Keysym keysym);
void UpdateSequence(Sequence *sequence, int deltaTicks, SoundManager *soundManager);
void DrawSequence(Sequence *sequence);

#endif /* Sequence_h */
