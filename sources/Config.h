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

#ifndef Config_h
#define Config_h

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define ELEMENT_NAME_SIZE 61
#define FILE_NAME_SIZE 31
#define LABEL_NAME_SIZE 41

#define MAX_PATH_SIZE 10

#define INVENTORY_BAR_SIZE 8

#ifdef TOUCH
#define INVENTORY_ITEM_OFFSET_X 50
#else
#define INVENTORY_ITEM_OFFSET_X 75
#endif

#define INVENTORY_ITEM_OFFSET_Y 68
#define INVENTORY_ITEM_WIDTH 48
#define INVENTORY_ITEM_HEIGHT 48
#define INVENTORY_ITEM_SPACE 23

#define NUM_SAVE_SLOTS 6
#define SLOT_NAME_SIZE 31

#define FADE_DURATION 300
#define SEQUENCE_FADE_DURATION 1000

#define MAX_TEXT_SPEED 2
#define DEFAULT_TEXT_SPEED 1

#define NUM_SOUND_SLOTS 8

#endif /* Config_h */
