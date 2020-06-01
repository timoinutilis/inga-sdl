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

#include "Global.h"

SDL_Renderer *globalRenderer = NULL;
SDL_Palette *globalPalette = NULL;
int MainPersonID = 0;
int ForegroundID = 0xFFFF;

int LayerDeco = 0;
int LayerObjects = 1;
int LayerFields = 2;
int LayerPersons = 3;
int LayerForeground = 4;
int LayerTop = 5;

void SetGlobalRenderer(SDL_Renderer *renderer) {
    globalRenderer = renderer;
}

SDL_Renderer *GetGlobalRenderer() {
    return globalRenderer;
}

void SetGlobalPalette(SDL_Palette *palette) {
    globalPalette = palette;
}

SDL_Palette *GetGlobalPalette(void) {
    return globalPalette;
}
