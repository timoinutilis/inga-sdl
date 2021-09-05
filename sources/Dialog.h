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

#ifndef Dialog_h
#define Dialog_h

#include <stdio.h>
#include "Image.h"
#include "Font.h"
#include "Enums.h"

typedef struct DialogItem {
    int id;
    const char *text;
    Image *image;
    Image *focusImage;
    Vector position;
    struct DialogItem *next;
} DialogItem;

typedef struct Dialog {
    DialogItem *rootItem;
    DialogItem *focusedItem;
} Dialog;

Dialog *CreateDialog(void);
void FreeDialog(Dialog *dialog);
bool HandleMouseInDialog(Dialog *dialog, int x, int y, ButtonState buttonState);
void DrawDialog(Dialog *dialog);
void AddDialogItem(Dialog *dialog, int id, const char *text, Font *font);
void RefreshDialog(Dialog *dialog);
void ResetDialog(Dialog *dialog);

#endif /* Dialog_h */
