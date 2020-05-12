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
