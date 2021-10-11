//
//  SDL_includes.h
//  Ermentrud iOS
//
//  Created by Timo Kloss on 10.09.21.
//

#ifndef SDL_includes_h
#define SDL_includes_h

#ifdef __APPLE__
#include "TargetConditionals.h"

#if TARGET_OS_IPHONE
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#else
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_mixer/SDL_mixer.h>
#endif

#else

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#endif

#endif /* SDL_includes_h */
