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

#ifndef Global_h
#define Global_h

#include <stdio.h>
#include <SDL2/SDL.h>

extern int MainPersonID;
extern int ForegroundID;

extern int LayerDeco;
extern int LayerObjects;
extern int LayerFields;
extern int LayerPersons;
extern int LayerForeground;
extern int LayerTop;

void SetGlobalRenderer(SDL_Renderer *renderer);
SDL_Renderer *GetGlobalRenderer(void);

void SetGlobalPalette(SDL_Palette *palette);
SDL_Palette *GetGlobalPalette(void);

#endif /* Global_h */
