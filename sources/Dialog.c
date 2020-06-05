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

#include "Dialog.h"
#include "Config.h"

Dialog *CreateDialog() {
    Dialog *dialog = calloc(1, sizeof(Dialog));
    if (!dialog) {
        printf("CreateDialog: Out of memory\n");
    } else {
    }
    return dialog;
}

void FreeDialog(Dialog *dialog) {
    if (!dialog) return;
    ResetDialog(dialog);
    free(dialog);
}

bool HandleMouseInDialog(Dialog *dialog, int x, int y, int buttonIndex) {
    if (!dialog || !dialog->rootItem) return false;
    dialog->focusedItem = NULL;
    DialogItem *item = dialog->rootItem;
    while (item) {
        if (y >= item->position.y && y < item->position.y + item->image->height) {
            dialog->focusedItem = item;
            break;
        }
        item = item->next;
    }
    return true;
}

void DrawDialog(Dialog *dialog) {
    if (!dialog || !dialog->rootItem) return;
    DialogItem *item = dialog->rootItem;
    while (item) {
        if (item == dialog->focusedItem) {
            DrawImage(item->focusImage, item->position);
        } else {
            DrawImage(item->image, item->position);
        }
        item = item->next;
    }
}

void AddDialogItem(Dialog *dialog, int id, const char *text, Font *font) {
    if (!dialog) return;
    DialogItem *item = calloc(1, sizeof(DialogItem));
    if (!item) {
        printf("AddDialogItem: Out of memory\n");
    } else {
        SDL_Color color = {132, 161, 235, 255};
        SDL_Color focusColor = {255, 255, 255, 255};
        item->id = id;
        item->text = text;
        item->image = CreateImageFromText(text, font, color);
        item->focusImage = CreateImageFromText(text, font, focusColor);
        item->next = dialog->rootItem;
        dialog->rootItem = item;
    }
}

void RefreshDialog(Dialog *dialog) {
    if (!dialog || !dialog->rootItem) return;
    DialogItem *item = dialog->rootItem;
    Vector position = MakeVector(0, 0);
    while (item) {
        position.x = (SCREEN_WIDTH - item->image->width) * 0.5;
        item->position = position;
        position.y += item->image->height + 4;
        item = item->next;
    }
    float offsetY = SCREEN_HEIGHT - position.y - 12;
    item = dialog->rootItem;
    while (item) {
        item->position.y += offsetY;
        item = item->next;
    }
}

void ResetDialog(Dialog *dialog) {
    if (!dialog) return;
    DialogItem *item = dialog->rootItem;
    while (item) {
        DialogItem *next = item->next;
        FreeImage(item->image);
        FreeImage(item->focusImage);
        free(item);
        item = next;
    }
    dialog->rootItem = NULL;
    dialog->focusedItem = NULL;
}
