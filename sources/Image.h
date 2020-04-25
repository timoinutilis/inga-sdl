//
//  Image.h
//  Inga
//
//  Created by Timo Kloss on 25/04/2020.
//  Copyright © 2020 Inutilis Software. All rights reserved.
//

#ifndef Image_h
#define Image_h

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

struct Frame {
    SDL_Rect sourceRect;
    int pivotX;
    int pivotY;
    int ticks;
};

struct Animation {
    int numFrames;
    struct Frame *frames;
};

struct Image {
    SDL_Texture *texture;
    SDL_Palette *palette;
    int width;
    int height;
    struct Animation *animation;
};

enum StripDirection {
    StripDirectionHorizontal,
    StripDirectionVertical
};

struct Image *LoadImageIBM(const char *filename, SDL_Renderer *renderer, SDL_Palette *defaultPalette, bool createMask);
void FreeImage(struct Image *image);
void DrawImage(struct Image *image, SDL_Renderer *renderer, int x, int y);
void DrawAnimationFrame(struct Image *image, SDL_Renderer *renderer, int x, int y, int index);

#endif /* Image_h */
