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
// along with LowRes NX.  If not, see <http://www.gnu.org/licenses/>.
//

#include "Font.h"
#include "Global.h"

Font *LoadFont(const char *filename, int size) {
    Font *font = NULL;
    
    char path[FILENAME_MAX];
    char *basePath = SDL_GetBasePath();
    if (basePath) {
        sprintf(path, "%s%s.ttf", basePath, filename);
    } else {
        sprintf(path, "%s.ttf", filename);
    }
    
    TTF_Font *ttfFont = TTF_OpenFont(path, size);
    if (!ttfFont) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    } else {
        font = calloc(1, sizeof(Font));
        if (!font) {
            printf("LoadFont: Out of memory\n");
        } else {
            font->ttfFont = ttfFont;
        }
    }
    return font;
}

void FreeFont(Font *font) {
    if (!font) return;
    TTF_CloseFont(font->ttfFont);
    free(font);
}

Image *CreateImageFromText(const char *text, Font *font, SDL_Color color) {
    if (!font) return NULL;
    
    Image *image = NULL;
        
    SDL_Color outlineColor = {0, 0, 0, 255};
    TTF_SetFontOutline(font->ttfFont, 1);
    SDL_Surface *surface = TTF_RenderText_Blended(font->ttfFont, text, outlineColor);
    
    TTF_SetFontOutline(font->ttfFont, 0);
    SDL_Surface *fgSurface = TTF_RenderText_Blended(font->ttfFont, text, color);
    
    if (!surface || !fgSurface) {
        printf("TTF_RenderText_Blended: %s\n", TTF_GetError());
    } else {
        SDL_Rect rect = {1, 1, fgSurface->w, fgSurface->h};
        SDL_BlitSurface(fgSurface, NULL, surface, &rect);
        SDL_Renderer *renderer = GetGlobalRenderer();
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            printf("SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        } else {
            image = calloc(1, sizeof(Image));
            if (!image) {
                printf("CreateImageFromText: Out of memory\n");
            } else {
                image->texture = texture;
                image->width = surface->w;
                image->height = surface->h;
            }
        }
    }
    if (surface) {
        SDL_FreeSurface(surface);
    }
    if (fgSurface) {
        SDL_FreeSurface(fgSurface);
    }
    return image;
}
