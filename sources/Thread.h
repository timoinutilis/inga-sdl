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

#ifndef Thread_h
#define Thread_h

#include <stdio.h>
#include <stdbool.h>
#include "Element.h"

typedef struct Game Game;

typedef enum Verb {
    VerbUse,
    VerbLook,
    VerbSay
} Verb;

typedef struct Thread {
    int id;
    bool isActive;
    unsigned long ptr;
    unsigned long subptr;
    unsigned long listeptr;
    unsigned long dialoglisteptr;
    unsigned long escptr;
    Element *talkingElement;
    int benutzt;
    int invbenutzt;
    int gesagt;
    int angesehen;
    struct Thread *next;
} Thread;

Thread *CreateThread(int id);
void FreeThread(Thread *thread);
void UpdateThread(Thread *thread, Game *game);
void EscapeThread(Thread *thread);
void RunThread(Thread *thread, unsigned long ptr);
void StartInteraction(Thread *thread, int id1, int id2, Verb verb);

#endif /* Thread_h */
