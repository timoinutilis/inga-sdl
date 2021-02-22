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

void FreeTrack(SoundManager *soundManager);


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
    
    Mix_HaltChannel(-1);
    
    FreeTrack(soundManager);
    
    for (int i = 0; i < NUM_SOUND_SLOTS; ++i) {
        FreeSound(soundManager, i);
    }
    
    free(soundManager);
}

void PlayTrack(SoundManager *soundManager, int number) {
    if (!soundManager || number == soundManager->currentTrackNumber) return;
    FreeTrack(soundManager);
    
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
    
    Mix_FadeOutMusic(1000);
    soundManager->currentTrackNumber = 0;
}

void FreeTrack(SoundManager *soundManager) {
    if (!soundManager) return;
    
    if (soundManager->music) {
        Mix_FreeMusic(soundManager->music);
        soundManager->music = NULL;
    }
}

void LoadSound(SoundManager *soundManager, int slot, const char *filename) {
    if (!soundManager) return;
    
    if (soundManager->soundFilenames[slot] != NULL && strcmp(filename, soundManager->soundFilenames[slot]) == 0) {
        // already loaded
        return;
    }

    FreeSound(soundManager, slot);
    
    char path[FILENAME_MAX];
    sprintf(path, "game/Sounds/%s.aiff", filename);
    
    Mix_Chunk *sound = Mix_LoadWAV(path);
    if (!sound) {
        printf("Mix_LoadWAV: %s\n", Mix_GetError());
    } else {
        soundManager->sounds[slot] = sound;
        soundManager->soundFilenames[slot] = filename;
    }
}

void FreeSound(SoundManager *soundManager, int slot) {
    if (!soundManager) return;
    
    Mix_Chunk *sound = soundManager->sounds[slot];
    if (sound) {
        Mix_FreeChunk(sound);
        soundManager->sounds[slot] = NULL;
    }
    soundManager->soundFilenames[slot] = NULL;
}

void SetSoundPosition(int channel, int volume, int pan) {
    int angle = ((90 * (pan - 50) / 50) + 360) % 360;
    int distance = (100 - volume) * 255 / 100;
    Mix_SetPosition(channel, angle, distance);
}

void PlaySound(SoundManager *soundManager, int slot, int volume, int pan) {
    if (!soundManager) return;
    
    Mix_Chunk *sound = soundManager->sounds[slot];
    if (!sound) return;
    
    SetSoundPosition(0, volume, pan);
    if (Mix_PlayChannel(0, sound, 0) == -1) {
        printf("Mix_PlayChannel: %s\n", Mix_GetError());
    }
}

void PlaySoundLoop(SoundManager *soundManager, int slot, int volume, int pan) {
    if (!soundManager) return;
    
    Mix_Chunk *sound = soundManager->sounds[slot];
    if (!sound) return;
    
    SetSoundPosition(1, volume, pan);
    if (Mix_PlayChannel(1, sound, -1) == -1) {
        printf("Mix_PlayChannel: %s\n", Mix_GetError());
    }
}

void StopSoundLoop(SoundManager *soundManager) {
    if (!soundManager) return;
    Mix_HaltChannel(1);
}
