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

#ifndef Thread_h
#define Thread_h

#include <stdio.h>
#include <stdbool.h>
#include "Element.h"

typedef struct Game Game;

typedef struct Thread {
    int id;
    bool isActive;
    unsigned long ptr;
    unsigned long subptr;
    unsigned long listeptr;
    unsigned long dialoglisteptr;
    unsigned long escptr;
    Element *talkingElement;
} Thread;

Thread *CreateThread(int id);
void FreeThread(Thread *thread);
void UpdateThread(Thread *thread, Game *game);

#endif /* Thread_h */
