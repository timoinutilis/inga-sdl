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

#ifndef Game_h
#define Game_h

#include <stdio.h>
#include <stdbool.h>
#include "SDL_includes.h"
#include "Location.h"
#include "Image.h"
#include "Element.h"
#include "Font.h"
#include "Script.h"
#include "Thread.h"
#include "GameState.h"
#include "InventoryBar.h"
#include "Dialog.h"
#include "Sequence.h"
#include "Fader.h"
#include "Menu.h"
#include "SlotList.h"
#include "GameConfig.h"
#include "SoundManager.h"
#include "Enums.h"

typedef struct Focus {
    const char *name;
    Image *image;
    Vector position;
} Focus;

typedef struct IdleScript {
    unsigned long idleTicks;
    unsigned long delay;
    unsigned long scriptPtr;
} IdleScript;

typedef enum GameAction {
    GameActionNone,
    GameActionOpenMenu,
    GameActionAskQuit
} GameAction;

typedef struct Game {
    GameConfig *config;
    Font *font;
    SDL_Cursor *cursorNormal;
    SDL_Cursor *cursorDrag;
    Script *script;
    GameState *gameState;
    Thread *mainThread;
    InventoryBar *inventoryBar;
    Dialog *dialog;
    Element *mainPerson;
    Location *location;
    Sequence *sequence;
    Focus focus;
    InventoryItemView draggingItemView;
    int selectedId;
    int draggedId;
    Verb selectedVerb;
    IdleScript idleScript;
    Fader fader;
    Menu *menu;
    SlotList *slotList;
    SoundManager *soundManager;
    FILE *logFile;
    GameAction actionAfterFadeOut;
    bool isPaused;
    Image *pauseImage;
#ifdef TOUCH
    Image *inventoryButtonImage;
#endif
} Game;

Game *CreateGame(GameConfig *config);
void FreeGame(Game *game);
void HandleMouseInGame(Game *game, int x, int y, ButtonState buttonState);
void HandleKeyInGame(Game *game, SDL_Keysym keysym);
void HandleGameCheat(Game *game, const char *cheat);
void UpdateGame(Game *game, int deltaTicks);
void DrawGame(Game *game);

void SetLocation(Game *game, int id, const char *background);
void SetGameState(Game *game, GameState *gameState);
void RefreshGameState(Game *game);
void SaveGameSlot(Game *game, int slot);
void LoadGameSlot(Game *game, int slot);
void AutosaveIfPossible(Game *game);
void SafeQuit(Game *game);

void MainPersonDidFinishWalking(Game *game);

#endif /* Game_h */
