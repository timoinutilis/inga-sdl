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

#ifndef Dialog_h
#define Dialog_h

#include <stdio.h>
#include "Image.h"
#include "Font.h"

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
bool HandleMouseInDialog(Dialog *dialog, int x, int y, int buttonIndex);
void DrawDialog(Dialog *dialog);
void AddDialogItem(Dialog *dialog, int id, const char *text, Font *font);
void RefreshDialog(Dialog *dialog);
void ResetDialog(Dialog *dialog);

#endif /* Dialog_h */
