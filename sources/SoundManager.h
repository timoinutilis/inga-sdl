//
// Copyright (c) 2021 Timo Kloss
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

#ifndef SoundManager_h
#define SoundManager_h

#include <stdio.h>
#include "SDL_includes.h"
#include "Config.h"

typedef struct SoundManager {
    Mix_Music *music;
    int currentTrackNumber;
    Mix_Chunk *sounds[NUM_SOUND_SLOTS];
    const char *soundFilenames[NUM_SOUND_SLOTS];
} SoundManager;

SoundManager *CreateSoundManager(void);
void FreeSoundManager(SoundManager *soundManager);

void PlayTrack(SoundManager *soundManager, int number);
void StopTrack(SoundManager *soundManager);

void LoadSound(SoundManager *soundManager, int slot, const char *filename);
void FreeSound(SoundManager *soundManager, int slot);
void PlaySound(SoundManager *soundManager, int slot, int volume, int pan);
void PlaySoundLoop(SoundManager *soundManager, int slot, int volume, int pan);
void StopSoundLoop(SoundManager *soundManager);

#endif /* SoundManager_h */
