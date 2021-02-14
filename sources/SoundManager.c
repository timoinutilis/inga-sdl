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

#include "SoundManager.h"

SoundManager *CreateSoundManager() {
    SoundManager *soundManager = calloc(1, sizeof(struct SoundManager));
    if (!soundManager) {
        printf("CreateSoundManager: Out of memory\n");
    } else {
    }
    return soundManager;
}

void FreeSoundManager(SoundManager *soundManager) {
    if (!soundManager) return;
    StopTrack(soundManager);
    free(soundManager);
}

void PlayTrack(SoundManager *soundManager, int number) {
    if (!soundManager && number != soundManager->currentTrackNumber) return;
    StopTrack(soundManager);
    
    char path[FILENAME_MAX];
    sprintf(path, "game/Tracks/Track%02d.ogg", number);
    
    soundManager->music = Mix_LoadMUS(path);
    if (!soundManager->music) {
        printf("Mix_LoadMUS: %s\n", Mix_GetError());
    } else {
        if (Mix_PlayMusic(soundManager->music, -1) == -1) {
            printf("Mix_PlayMusic: %s\n", Mix_GetError());
        }
    }
    
    soundManager->currentTrackNumber = number;
}

void StopTrack(SoundManager *soundManager) {
    if (!soundManager) return;
    
    if (soundManager->music) {
        Mix_FreeMusic(soundManager->music);
        soundManager->music = NULL;
    }
    
    soundManager->currentTrackNumber = 0;
}
