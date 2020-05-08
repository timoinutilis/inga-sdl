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

#include "Image.h"
#include "Global.h"

typedef struct IBMColor {
    Uint8 r;
    Uint8 g;
    Uint8 b;
} IBMColor;

Animation *CreateAnimationFromStrip(int numFrames, enum StripDirection direction, int width, int height, int pivotX, int pivotY, int ticks);
void FreeAnimation(Animation *animation);

Image *LoadImage(const char *filename, SDL_Palette *defaultPalette, bool createMask, bool keepSurface) {
    Image *image = NULL;
    SDL_Renderer *renderer = GetGlobalRenderer();
    
    char path[FILENAME_MAX];
    sprintf(path, "game/BitMaps/%s.ibm", filename);
    
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    if (!file) {
        printf("LoadImageIBM: %s\n", SDL_GetError());
    } else {
        Uint32 head = SDL_ReadBE32(file);
        if (head != 0x49424D38) {
            printf("LoadImageIBM: Invalid file format\n");
        } else {
            Uint16 width = SDL_ReadBE16(file);
            Uint16 height = SDL_ReadBE16(file);
            Uint8 depth = SDL_ReadU8(file);
            SDL_ReadU8(file);
            Uint16 bytesPerRow = SDL_ReadBE16(file);
            Uint8 flags = SDL_ReadU8(file);
            SDL_ReadU8(file);
            SDL_ReadBE32(file);
            SDL_ReadBE32(file);
            
            IBMColor *ibmColors = NULL;
            Animation *animation = NULL;
            if (flags & 1) {
                // color palette
                ibmColors = calloc(256, sizeof(IBMColor));
                if (ibmColors) {
                    SDL_RWread(file, ibmColors, sizeof(IBMColor), 256);
                } else {
                    SDL_RWseek(file, sizeof(IBMColor) * 256, RW_SEEK_CUR);
                }
            }
            if (flags & 2) {
                // animation
                Uint16 numFrames = SDL_ReadBE16(file);
                Uint16 direction = SDL_ReadBE16(file);
                Uint16 frameWidth = SDL_ReadBE16(file);
                Uint16 frameHeight = SDL_ReadBE16(file);
                Uint16 pivotX = SDL_ReadBE16(file);
                Uint16 pivotY = SDL_ReadBE16(file);
                Uint16 pausesPerFrame = SDL_ReadBE16(file);
                SDL_ReadBE32(file);
                SDL_ReadBE32(file);
                int ticks = (pausesPerFrame + 1) * 50; // based on 20 FPS
                animation = CreateAnimationFromStrip(numFrames, direction, frameWidth, frameHeight, pivotX, pivotY, ticks);
            }
            
            size_t size = bytesPerRow * height;
            Uint8 *pixels = calloc(sizeof(Uint8), size);
            if (!pixels) {
                printf("LoadImageIBM: Out of memory\n");
            } else {
                SDL_RWread(file, pixels, sizeof(Uint8), size);
                SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, depth, bytesPerRow, SDL_PIXELFORMAT_INDEX8);
                if (!surface) {
                    printf("LoadImageIBM: Create surface failed\n");
                } else {
                    SDL_Palette *surfacePalette = surface->format->palette;
                    if (ibmColors) {
                        SDL_Color colors[surfacePalette->ncolors];
                        for (int i = 0; i < surfacePalette->ncolors; i++) {
                            SDL_Color *sdlColor = &colors[i];
                            IBMColor *ibmColor = &ibmColors[i];
                            sdlColor->r = ibmColor->r;
                            sdlColor->g = ibmColor->g;
                            sdlColor->b = ibmColor->b;
                            sdlColor->a = 255;
                        }
                        SDL_SetPaletteColors(surfacePalette, colors, 0, surfacePalette->ncolors);
                    } else if (defaultPalette) {
                        SDL_SetPaletteColors(surfacePalette, defaultPalette->colors, 0, surfacePalette->ncolors);
                    }
                    if (createMask) {
                        SDL_Color transparentColor = surfacePalette->colors[0];
                        Uint32 key = SDL_MapRGB(surface->format, transparentColor.r, transparentColor.g, transparentColor.b);
                        SDL_SetColorKey(surface, SDL_TRUE, key);
                    }
                    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
                    if (!texture) {
                        printf("LoadImageIBM: Create texture failed\n");
                    } else {
                        image = calloc(1, sizeof(Image));
                        if (!image) {
                            printf("LoadImageIBM: Out of memory\n");
                        } else {
                            image->texture = texture;
                            if (keepSurface) {
                                image->surface = surface;
                            }
                            image->animation = animation;
                            image->width = width;
                            image->height = height;
                        }
                    }
                    if (!keepSurface) {
                        SDL_FreeSurface(surface);
                    }
                }
                free(pixels);
            }
            if (ibmColors) {
                free(ibmColors);
            }
            if (animation && !image) {
                FreeAnimation(animation);
            }
        }
        SDL_RWclose(file);
    }
    return image;
}

Image *LoadMaskedImage(const char *filename, Image *sourceImage) {
    if (!sourceImage || !sourceImage->surface) return NULL;
    Image *image = NULL;
    SDL_Renderer *renderer = GetGlobalRenderer();
    
    char path[FILENAME_MAX];
    sprintf(path, "game/BitMaps/%s.imp", filename);
    
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    if (!file) {
        printf("LoadImageIMP: %s\n", SDL_GetError());
    } else {
        Uint32 head = SDL_ReadBE32(file);
        if (head != 0x494D5031) {
            printf("LoadImageIMP: Invalid file format\n");
        } else {
            Uint16 width = SDL_ReadBE16(file);
            Uint16 height = SDL_ReadBE16(file);
            Uint32 size = SDL_ReadBE32(file);
            
            Uint8 *pixels = calloc(sizeof(Uint8), size);
            if (!pixels) {
                printf("LoadImageIMP: Out of memory\n");
            } else {
                SDL_RWread(file, pixels, sizeof(Uint8), size);
                SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, 1, size / height, SDL_PIXELFORMAT_INDEX1MSB);
                if (!surface) {
                    printf("LoadImageIMP: Create surface failed\n");
                } else {
                    SDL_Palette *surfacePalette = surface->format->palette;
                    SDL_Color colors[] = {{255, 0, 255, 255}, {255, 255, 255, 255}};
                    SDL_SetPaletteColors(surfacePalette, colors, 0, 2);
                    Uint32 key = SDL_MapRGB(surface->format, 255, 255, 255);
                    SDL_SetColorKey(surface, SDL_TRUE, key);
                    
                    SDL_Surface *targetSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
                    if (!targetSurface) {
                        printf("LoadImageIMP: Create surface failed\n");
                    } else {
                        SDL_BlitSurface(sourceImage->surface, NULL, targetSurface, NULL);
                        SDL_BlitSurface(surface, NULL, targetSurface, NULL);
                        Uint32 key2 = SDL_MapRGB(targetSurface->format, 255, 0, 255);
                        SDL_SetColorKey(targetSurface, SDL_TRUE, key2);
                        
                        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, targetSurface);
                        if (!texture) {
                            printf("LoadImageIMP: Create texture failed\n");
                        } else {
                            image = calloc(1, sizeof(Image));
                            image->texture = texture;
                            image->width = width;
                            image->height = height;
                        }
                        SDL_FreeSurface(targetSurface);
                    }
                    SDL_FreeSurface(surface);
                }
                free(pixels);
            }
        }
        SDL_RWclose(file);
    }
    return image;
}

void FreeImage(Image *image) {
    if (!image) return;
    SDL_DestroyTexture(image->texture);
    SDL_FreeSurface(image->surface);
    FreeAnimation(image->animation);
    free(image);
}

void DrawImage(Image *image, Vector position) {
    if (!image) return;
    SDL_Renderer *renderer = GetGlobalRenderer();
    SDL_Rect src = {0, 0, image->width, image->height};
    SDL_Rect dst = {position.x, position.y, image->width, image->height};
    SDL_RenderCopy(renderer, image->texture, &src, &dst);
}

Animation *CreateAnimationFromStrip(int numFrames, enum StripDirection direction, int width, int height, int pivotX, int pivotY, int ticks) {
    Animation *animation = NULL;
    
    Frame *frames = calloc(numFrames, sizeof(Frame));
    if (frames) {
        SDL_Rect rect = {0, 0, width, height};
        for (int i = 0; i < numFrames; i++) {
            Frame *frame = &frames[i];
            frame->sourceRect = rect;
            frame->pivot.x = pivotX;
            frame->pivot.y = pivotY;
            frame->ticks = ticks;
            switch (direction) {
                case StripDirectionHorizontal:
                    rect.x += width + 1;
                    break;
                case StripDirectionVertical:
                    rect.y += height + 1;
                    break;
            }
        }
        animation = calloc(1, sizeof(Animation));
        if (animation) {
            animation->numFrames = numFrames;
            animation->frames = frames;
        } else {
            free(frames);
        }
    }
    return animation;
}

void FreeAnimation(Animation *animation) {
    if (!animation) return;
    free(animation->frames);
    free(animation);
}

void DrawAnimationFrame(Image *image, Vector position, int index) {
    if (!image || !image->animation) return;
    SDL_Renderer *renderer = GetGlobalRenderer();
    Frame *frame = &image->animation->frames[index];
    SDL_Rect dst = {position.x - frame->pivot.x, position.y - frame->pivot.y, frame->sourceRect.w, frame->sourceRect.h};
    SDL_RenderCopy(renderer, image->texture, &frame->sourceRect, &dst);
}
