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

#include "Font.h"
#include "Global.h"

Font *LoadFont(const char *filename, const int size) {
    Font *font = NULL;
    
    char path[FILENAME_MAX];
    memset(path,0, FILENAME_MAX);
    char *basePath = SDL_GetBasePath();
    if (basePath) {
        sprintf(path, "%s/game/%s.ttf", basePath, filename);
    } else {
        sprintf(path, "game/%s.ttf", filename);
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
    SDL_free(basePath);
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
