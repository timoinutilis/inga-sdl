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

#include "Image.h"
#include "Global.h"
#include "Utils.h"

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
    GameFilePath(path, "BitMaps", filename, "ibm");
    
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    if (!file) {
        printf("LoadImage: %s\n", SDL_GetError());
    } else {
        Uint32 head = SDL_ReadBE32(file);
        if (head != 0x49424D38) {
            printf("LoadImage: Invalid file format\n");
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
            
            const size_t size = bytesPerRow * height;
            Uint8 *pixels = calloc(sizeof(Uint8), size);
            if (!pixels) {
                printf("LoadImage: Out of memory\n");
            } else {
                SDL_RWread(file, pixels, sizeof(Uint8), size);
                SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, depth, bytesPerRow, SDL_PIXELFORMAT_INDEX8);
                if (!surface) {
                    printf("LoadImage: Create surface failed\n");
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
                        printf("LoadImage: Create texture failed\n");
                    } else {
                        image = calloc(1, sizeof(Image));
                        if (!image) {
                            printf("LoadImage: Out of memory\n");
                        } else {
                            image->texture = texture;
                            if (keepSurface) {
                                image->surface = surface;
                                image->pixel_buffer=pixels;
                            }else{
                                image->surface=NULL;
                                free(pixels);
                                image->pixel_buffer=NULL;
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
//                free(pixels); // don't free pixels because they are used by SDL_Surface! (ageisler)
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
    
    // truncate after "_": For example image files "House", "House_1", "House_2" all use mask file "House"
    char maskFilename[FILENAME_MAX];
    strcpy(maskFilename, filename);
    char *p = strrchr(maskFilename, '_');
    if (p) {
        *p = 0;
    }
    
    char path[FILENAME_MAX];
    GameFilePath(path, "BitMaps", maskFilename, "imp");
    
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    if (!file) {
        printf("LoadMaskedImage: %s\n", SDL_GetError());
    } else {
        Uint32 head = SDL_ReadBE32(file);
        if (head != 0x494D5031) {
            printf("LoadMaskedImage: Invalid file format\n");
        } else {
            Uint16 width = SDL_ReadBE16(file);
            Uint16 height = SDL_ReadBE16(file);
            Uint32 size = SDL_ReadBE32(file);
            
            Uint8 *pixels = calloc(sizeof(Uint8), size);
            if (!pixels) {
                printf("LoadMaskedImage: Out of memory\n");
            } else {
                SDL_RWread(file, pixels, sizeof(Uint8), size);
                SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, 1, size / height, SDL_PIXELFORMAT_INDEX1MSB);
                if (!surface) {
                    printf("LoadMaskedImage: Create surface failed\n");
                } else {
                    SDL_Palette *surfacePalette = surface->format->palette;
                    SDL_Color colors[] = {{255, 0, 255, 255}, {255, 255, 255, 255}};
                    SDL_SetPaletteColors(surfacePalette, colors, 0, 2);
                    Uint32 key = SDL_MapRGB(surface->format, 255, 255, 255);
                    SDL_SetColorKey(surface, SDL_TRUE, key);
                    
                    SDL_Surface *targetSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
                    if (!targetSurface) {
                        printf("LoadMaskedImage: Create surface failed\n");
                    } else {
                        SDL_BlitSurface(sourceImage->surface, NULL, targetSurface, NULL);
                        SDL_BlitSurface(surface, NULL, targetSurface, NULL);
                        Uint32 key2 = SDL_MapRGB(targetSurface->format, 255, 0, 255);
                        SDL_SetColorKey(targetSurface, SDL_TRUE, key2);
                        
                        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, targetSurface);
                        if (!texture) {
                            printf("LoadMaskedImage: Create texture failed\n");
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
    free(image->pixel_buffer);
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
