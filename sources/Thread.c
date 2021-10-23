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

#include "Thread.h"
#include "Game.h"
#include "Global.h"
#include "Cursor.h"

unsigned long LaufeINGA(Thread *thread, Game *game, unsigned long ptr, bool *wieder);
unsigned short peekv(Game *game, unsigned long pointer);

Thread *CreateThread(int id) {
    Thread *thread = calloc(1, sizeof(struct Thread));
    if (!thread) {
        printf("CreateThread: Out of memory\n");
    } else {
        thread->id = id;
        thread->isActive = true;
    }
    return thread;
}

void FreeThread(Thread *thread) {
    if (!thread) return;
    free(thread);
}

void UpdateThread(Thread *thread, Game *game) {
    if (!thread || !thread->isActive || !game || !game->script) return;
    bool cont = true;
    while (thread->isActive && cont) {
        thread->ptr = LaufeINGA(thread, game, thread->ptr, &cont);
    }
}

unsigned long LaufeINGA(Thread *thread, Game *game, unsigned long ptr, bool *wieder) {
    if (!thread) return ptr;
    
    Script *script = game->script;
    unsigned short opc;
    unsigned long use;

    opc = peekv(game, ptr);
    *wieder = true;

    /*==Einrichtung==*/
    if (opc == 1) { //Einrichtung.
        if (game->fader.state != FaderStateClosed) {
            if (game->fader.state != FaderStateFadingOut) {
                FadeOut(&game->fader);
            }
            *wieder = false;
            return(ptr);
        }
        SetLocation(game, peekv(game, ptr + 2), peeks(script, ptr + 4));
        StopSoundLoop(game->soundManager);
//        MeldungAbbruch();
        
        int trackNumber = peekv(game, ptr + 8);
        if (trackNumber > 0) {
            PlayTrack(game->soundManager, trackNumber);
        } else {
            StopTrack(game->soundManager);
        }
        return(ptr + 10);
    }
    if (opc == 2) { //EinrichtungEnde.
        UpdateElementVisibilities(game->location, game->gameState);
        FadeIn(&game->fader);
        return(ptr + 2);
    }
    if (opc == 83) { //LadeBild.
        LoadLocationBackground(game->location, peeks(script, ptr + 2));
        return(ptr + 6);
    }
    if (opc == 3) { //Feld.
        int id = peekv(game, ptr + 2);
        Element *element = CreateElement(id);
        if (element) {
            element->layer = LayerFields;
            element->selectionRect = MakeRectFromTo(peekv(game, ptr + 8), peekv(game, ptr + 10), peekv(game, ptr + 12), peekv(game, ptr + 14));
            strcpy(element->name, peeks(script, ptr + 4));
            element->target = MakeVector(peekv(game, ptr + 16), peekv(game, ptr + 18));
            element->isSelectable = true;
            AddElement(game->location, element);
        }
        SetVisibility(game->gameState, game->location->id, id, (bool)peekv(game, ptr + 20), true);
        return(ptr + 22);
    }
    if (opc == 4) { //FeldUnsichtbar.
        int id = peekv(game, ptr + 2);
        Element *element = GetElement(game->location, id);
        if (element) {
            element->isVisible = false;
        }
        SetVisibility(game->gameState, game->location->id, id, false, false);
        return(ptr + 4);
    }
    if (opc == 5) { //Zierde.
        int id = peekv(game, ptr + 2);
        Element *element = CreateElement(id);
        if (element) {
            element->layer = LayerDeco;
            element->position = MakeVector(peekv(game, ptr + 8), peekv(game, ptr + 10));
            element->image = LoadImage(peeks(script, ptr + 4), game->location->image->surface->format->palette, false, false);
            AddElement(game->location, element);
        }
        SetVisibility(game->gameState, game->location->id, id, (bool)peekv(game, ptr + 12), true);
        return(ptr + 14);
    }
    if (opc == 6) { //ZierdeUnsichtbar.
        int id = peekv(game, ptr + 2);
        Element *element = GetElement(game->location, id);
        if (element) {
            element->isVisible = false;
        }
        SetVisibility(game->gameState, game->location->id, id, false, false);
        return(ptr + 4);
    }
    if (opc == 7) { //Objekt.
        int id = peekv(game, ptr + 2);
        Element *element = CreateElement(id);
        if (element) {
            element->layer = LayerObjects;
            element->position = MakeVector(peekv(game, ptr + 12), peekv(game, ptr + 14));
            element->image = LoadImage(peeks(script, ptr + 8), game->location->image->surface->format->palette, false, false);
            strcpy(element->name, peeks(script, ptr + 4));
            element->target = MakeVector(peekv(game, ptr + 16), peekv(game, ptr + 18));
            element->isSelectable = true;
            AddElement(game->location, element);
        }
        SetVisibility(game->gameState, game->location->id, id, (bool)peekv(game, ptr + 20), true);
        return(ptr + 22);
    }
    if (opc == 8) { //ObjektUnsichtbar.
        int id = peekv(game, ptr + 2);
        Element *element = GetElement(game->location, id);
        if (element) {
            element->isVisible = false;
        }
        SetVisibility(game->gameState, game->location->id, id, false, false);
        return(ptr + 4);
    }
    if (opc == 62) { //ObjektStandbild.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element) {
            element->frameIndex = peekv(game, ptr + 4);
            element->isPaused = true;
        }
        return(ptr + 6);
    }
    if (opc == 63) { //ObjektStandbildAus.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element) {
            element->isPaused = false;
        }
        return(ptr + 4);
    }
    if (opc == 9) { //Person.
        int id = peekv(game, ptr + 2);
        Element *element = CreateElement(id);
        if (element) {
            element->layer = LayerPersons;
            element->position = MakeVector(peekv(game, ptr + 12), peekv(game, ptr + 14));
            element->imageSet = LoadImageSet(peeks(script, ptr + 8), game->location->image->surface->format->palette, true);
            strcpy(element->name, peeks(script, ptr + 4));
            element->isSelectable = element->name[0] != 0;
            AddElement(game->location, element);
        }
        SetVisibility(game->gameState, game->location->id, id, (bool)peekv(game, ptr + 16), true);
        return(ptr + 18);
    }
    if (opc == 82) { //PersonProg.
        Thread *thread = CreateThread(peekv(game, ptr + 2));
        if (thread) {
            thread->ptr = ptr + 8;
            thread->isActive = true;
            AddThread(game->location, thread);
        }
        return(peekl(script, ptr + 4));
    }
    if (opc == 10) { //PersonUnsichtbar.
        int id = peekv(game, ptr + 2);
        Element *element = GetElement(game->location, id);
        if (element) {
            element->isVisible = false;
            element->action = ElementActionIdle;
        }
        if (id != MainPersonID) {
            SetVisibility(game->gameState, game->location->id, id, false, false);
        }
        return(ptr + 4);
    }
    if (opc == 93) { //PersonVgMaskeAktiv
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element) {
            element->layer = LayerPersons;
        }
        return(ptr + 4);
    }
    if (opc == 94) { //PersonVgMaskeInaktiv
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element) {
            element->layer = LayerTop;
        }
        return(ptr + 4);
    }
    if (opc == 76) { //Vorladen.
//        Vorladen(peekv(game, ptr + 2), peekv(game, ptr + 4));
        return(ptr + 6);
    }
    if (opc == 77) { //Freigeben.
//        Freigeben(peekv(game, ptr + 2), peekv(game, ptr + 4));
        return(ptr + 6);
    }
    if (opc == 42) { //Sichtbar.
        int locationId = peekv(game, ptr + 2);
        int elementId = peekv(game, ptr + 4);
        if (game->location->id == locationId) {
            Element *element = GetElement(game->location, elementId);
            if (element) {
                element->isVisible = true;
            }
        }
        if (elementId != MainPersonID) {
            SetVisibility(game->gameState, locationId, elementId, true, false);
        }
        return(ptr + 6);
    }
    if (opc == 18) { //Unsichtbar.
        int locationId = peekv(game, ptr + 2);
        int elementId = peekv(game, ptr + 4);
        if (game->location->id == locationId) {
            Element *element = GetElement(game->location, elementId);
            if (element) {
                element->isVisible = false;
            }
        }
        if (elementId != MainPersonID) {
            SetVisibility(game->gameState, locationId, elementId, false, false);
        }
        return(ptr + 6);
    }
    if (opc == 72) { //ilkOben.
        NavigationMapDrawLine(game->location->navigationMap, 0, peekv(game, ptr + 2), peekv(game, ptr + 4), peekv(game, ptr + 6), peekv(game, ptr + 8));
        return(ptr + 10);
    }
    if (opc == 73) { //ilkUnten.
        NavigationMapDrawLine(game->location->navigationMap, 1, peekv(game, ptr + 2), peekv(game, ptr + 4), peekv(game, ptr + 6), peekv(game, ptr + 8));
        return(ptr + 10);
    }
    if (opc == 74) { //LadeLaufkarte.
        LoadLocationNavigationMap(game->location, peeks(script, ptr + 2));
        return(ptr + 6);
    }
    
    /* == Personen == */
    if (opc == 70) { //Lauftempo.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element) {
            if (element->action != ElementActionIdle) {
                *wieder = false;
                return(ptr);
            }
            int factor = peekv(game, ptr + 4);
            if (factor < 1) {
                factor = 1;
            }
            element->speed = factor * 20; // pxl/s
        }
        return(ptr + 6);
    }
    if (opc == 11) { //Laufe.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element) {
            if (element->action != ElementActionIdle) {
                *wieder = false;
                return(ptr);
            }
            ElementMoveTo(element, peekv(game, ptr + 4), peekv(game, ptr + 6), peekv(game, ptr + 8), false);
        }
        return(ptr + 10);
    }
    if (opc == 12) { //LaufeDirekt.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element) {
            if (element->action != ElementActionIdle) {
                *wieder = false;
                return(ptr);
            }
            //TODO: skip navigation
            ElementMoveTo(element, peekv(game, ptr + 4), peekv(game, ptr + 6), peekv(game, ptr + 8), true);
        }
        return(ptr + 10);
    }
    if (opc == 13) { //Rede.
        if (thread->talkingElement) {
            if (thread->talkingElement->action == ElementActionTalk) {
                *wieder = false;
                return(ptr);
            } else {
                thread->talkingElement = NULL;
            }
        }
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element) {
            if (element->action != ElementActionIdle) {
                *wieder = false;
                return(ptr);
            }
            ElementTalk(element, peeks(script, ptr + 6), peekv(game, ptr + 4), game->font, game->gameState->textSpeed);
            thread->talkingElement = element;
            // sound: peeks(script, ptr + 10)
        }
        return(ptr + 14);
    }
    if (opc == 14) { //Anim.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element) {
            if (element->action != ElementActionIdle) {
                *wieder = false;
                return(ptr);
            }
            ElementAnimate(element, peekv(game, ptr + 4), peekv(game, ptr + 6));
        }
        return(ptr + 8);
    }
    if (opc == 41) { //AnimNehmen.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element) {
            if (element->action != ElementActionIdle) {
                *wieder = false;
                return(ptr);
            }
            int takeElementId = peekv(game, ptr + 4);
            Element *takeElement = GetElement(game->location, takeElementId);
            ElementTake(element, peekv(game, ptr + 6), takeElement, peekv(game, ptr + 8));
            SetVisibility(game->gameState, game->location->id, takeElementId, false, false);
        }
        return(ptr + 10);
    }
    if (opc == 84) { //NichtsProg.
        game->idleScript.delay = peekv(game, ptr + 2) * 50;
        game->idleScript.scriptPtr = peekl(script, ptr + 4);
        return(ptr + 8);
    }
    if (opc == 86) { //LöscheNichtsProg.
        game->idleScript.delay = 0;
        game->idleScript.scriptPtr = 0;
        return(ptr + 2);
    }
    if (opc == 15) { //Richtung.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element) {
            if (element->action != ElementActionIdle) {
                *wieder = false;
                return(ptr);
            }
            ElementSetDirection(element, DirectionForSide(peekv(game, ptr + 4)));
        }
        return(ptr + 6);
    }
    if (opc == 32) { //RichteAuf.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element) {
            if (element->action != ElementActionIdle) {
                *wieder = false;
                return(ptr);
            }
            Element *targetElement = GetElement(game->location, peekv(game, ptr + 4));
            ElementLookTo(element, targetElement->position.x, targetElement->position.y, 0);
        }
        return(ptr + 6);
    }
    if (opc == 33) { //RichteAufeinander.
        Element *element1 = GetElement(game->location, peekv(game, ptr + 2));
        Element *element2 = GetElement(game->location, peekv(game, ptr + 4));
        if (element1 && element2) {
            if (element1->action != ElementActionIdle || element2->action != ElementActionIdle) {
                *wieder = false;
                return(ptr);
            }
            ElementLookTo(element1, element2->position.x, element2->position.y, 0);
            ElementLookTo(element2, element1->position.x, element1->position.y, 0);
        }
        return(ptr + 6);
    }
    if (opc == 16) { //StelleAuf.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element) {
            if (element->action != ElementActionIdle) {
                *wieder = false;
                return(ptr);
            }
            element->position.x = peekv(game, ptr + 4);
            element->position.y = peekv(game, ptr + 6);
            ElementSetDirection(element, DirectionForSide(peekv(game, ptr + 8)));
        }
        return(ptr + 10);
    }
    if (opc == 17) { //WarteAuf.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element && element->action != ElementActionIdle) {
            *wieder = false;
            return(ptr);
        }
        return(ptr + 4);
    }
    if (opc == 37) { //WarteAufAnim.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        int frame = peekv(game, ptr + 4);
        if (element && element->image && element->image->animation && element->image->animation->numFrames > frame) {
            if (element->frameIndex >= frame) {
                return(ptr + 6);
            } else {
                *wieder = false;
                return(ptr);
            }
        } else {
            return(ptr + 6);
        }
    }
    if (opc == 30) { //Aktiv.
        Thread *thread = GetThread(game->location, peekv(game, ptr + 2));
        if (thread) {
            thread->isActive = true;
        }
        return(ptr + 4);
    }
    if (opc == 31) { //Inaktiv.
        int id = peekv(game, ptr + 2);
        Element *element = GetElement(game->location, id);
        ElementStop(element);
        Thread *thread = GetThread(game->location, id);
        if (thread) {
            thread->isActive = false;
        }
        return(ptr + 4);
    }
    if (opc == 92) { //WechsleHauptIPE.
//        person = SucheIDPerson(0);
//        if (person->aktion == AKT_NICHTS) {
//            ErsetzeHauptpersonIPE(peeks(script, ptr + 2));
//        } else return(ptr);
        return(ptr + 6);
    }
    if (opc == 56) { //PersonenAnims.
        Element *element = GetElement(game->location, MainPersonID);
        SetDefaultIdleImageId(element, peekv(game, ptr + 2));
        SetDefaultWalkImageId(element, peekv(game, ptr + 4));
        return(ptr + 6);
    }
    if (opc == 87) { //StandAnim.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        SetDefaultIdleImageId(element, peekv(game, ptr + 4));
        return(ptr + 6);
    }

    /* == Systemsteuerung == */
    if (opc == 59) { //Farben.
//        sysfar[0] = peekv(game, ptr + 2);
//        sysfar[1] = peekv(game, ptr + 4);
//        sysfar[2] = peekv(game, ptr + 6);
//        sysfar[3] = peekv(game, ptr + 8);
//        sysfar[4] = peekv(game, ptr + 10);
//        sysfar[5] = peekv(game, ptr + 12);
        return(ptr + 14);
    }
    if (opc == 19) { //Springe.
        return(peekl(script, ptr + 2));
    }
    if (opc == 29) { //SpringeOrt.
        unsigned long locationPtr = peekl(script, ptr + 2);
        Label *label = GetLabelWithPtr(game->script, locationPtr);
        if (label) {
            strcpy(game->gameState->locationLabel, label->name);
        }
        game->gameState->startPosition = MakeVector(peekv(game, ptr + 6), peekv(game, ptr + 8));
        game->gameState->startDirection = DirectionForSide(peekv(game, ptr + 10));
#ifdef AUTOSAVE
        SaveGameSlot(game, 0);
#endif
        return(locationPtr);
    }
    if (opc == 43) { //SpringeSub.
        if (thread->subptr == 0) {
            thread->subptr = ptr + 6;
            return(peekl(script, ptr + 2));
        } //else Fehler(6, "Verschachteltes 'Sub'");
    }
    if (opc == 44) { //Zurück.
        if (thread->subptr > 0) {
            use = thread->subptr; thread->subptr = 0;
            return(use);
        } //else Fehler(6, "'Zurück' ohne 'Sub'");
    }
    if (opc == 57) { //SpringeEsc.
        thread->escptr = peekl(script, ptr + 2);
        return(ptr + 6);
    }
    if (opc == 58) { //EscEnde.
        thread->escptr = 0;
        return(ptr + 2);
    }
    if (opc == 22) { //Stopp.
        SetCursor(game->cursorNormal);
        thread->isActive = false;
        return(ptr + 2);
    }
    if (opc == 88) { //ListeAbbruch.
        thread->benutzt = 0;
        thread->invbenutzt = 0;
        thread->angesehen = 0;
        thread->gesagt = 0;
        return(ptr + 2);
    }
    if (opc == 23) { //Liste.
        SetCursor(game->cursorNormal);
        thread->listeptr = ptr + 2;
        thread->isActive = false;
        return(ptr + 2);
    }
    if (opc == 39) { //Dialogliste.
        SetCursor(game->cursorNormal);
        thread->dialoglisteptr = ptr + 2;
        thread->isActive = false;
        RefreshDialog(game->dialog);
        return(ptr + 2);
    }
    if (opc == 26) { //WennBenutzt.
        if ((thread->benutzt == 0) || (thread->invbenutzt > 0)) return(peekl(script, ptr + 4));
        unsigned short param = peekv(game, ptr + 2);
        if (param == thread->benutzt) {
            return(ptr + 8);
        }
        if (param == 0) {
            if (game->logFile) {
                fprintf(game->logFile, "> Default handler\n");
            }
            return(ptr + 8);
        }
        return(peekl(script, ptr + 4));
    }
    if (opc == 27) { //WennBenutztMit.
        if ((thread->benutzt > 0) && (thread->invbenutzt > 0)) {
            unsigned short param1 = peekv(game, ptr + 2);
            unsigned short param2 = peekv(game, ptr + 4);
            if (((param1 == thread->benutzt) && (param2 == thread->invbenutzt)) ||
                ((param1 == thread->invbenutzt) && (param2 == thread->benutzt))) {
                return(ptr + 10);
            }
            if ((param1 == 0) && ((param2 == thread->benutzt) || (param2 == thread->invbenutzt))) {
                return(ptr + 10);
            }
            if ((param1 == 0) && (param2 == 0)) {
                if (game->logFile) {
                    fprintf(game->logFile, "> Default handler\n");
                }
                return(ptr + 10);
            }
        }
        return(peekl(script, ptr + 6));
    }
    if (opc == 28) { //WennAngesehen.
        if (thread->angesehen == 0) return(peekl(script, ptr + 4));
        if ((peekv(game, ptr + 2) == thread->angesehen) || (peekv(game, ptr + 2) == 0)) {
            return(ptr + 8);
        } else {
            return(peekl(script, ptr + 4));
        }
    }
    if (opc == 40) { //WennGesagt.
        if (peekv(game, ptr + 2) == thread->gesagt) {
            return(ptr + 8);
        } else {
            return(peekl(script, ptr + 4));
        }
    }
    if (opc == 34) { //Sequenz.
        if (game->fader.state != FaderStateClosed) {
            if (game->fader.state != FaderStateFadingOut) {
                FadeOut(&game->fader);
            }
            *wieder = false;
            return(ptr);
        }
        StopSoundLoop(game->soundManager);
        RefreshGameState(game);
        const char *filename = peeks(script, ptr + 2);
#ifdef TOUCH
        if (strcmp(filename, "SeqTutorial") == 0) {
            game->sequence = LoadSequence("SeqTutorialMobile");
        } else {
            game->sequence = LoadSequence(filename);
        }
#else
        game->sequence = LoadSequence(filename);
#endif
        *wieder = false;
        return(ptr + 6);
    }
    if (opc == 47) { //CDNummer.
        PlayTrack(game->soundManager, peekv(game, ptr + 2));
        return(ptr + 4);
    }
    if (opc == 48) { //CDStopp.
        StopTrack(game->soundManager);
        return(ptr + 2);
    }
    if (opc == 61) { //LadeSound.
        LoadSound(game->soundManager, peekv(game, ptr + 2), peeks(script, ptr + 4));
        return(ptr + 8);
    }
    if (opc == 64) { //EntferneSound.
        FreeSound(game->soundManager, peekv(game, ptr + 2));
        return(ptr + 4);
    }
    if (opc == 66) { //SpieleSound.
        PlaySound(game->soundManager, peekv(game, ptr + 2), peekv(game, ptr + 4), peekv(game, ptr + 6));
        return(ptr + 8);
    }
    if (opc == 95) { //SpieleSoundSchleife.
        PlaySoundLoop(game->soundManager, peekv(game, ptr + 2), peekv(game, ptr + 4), peekv(game, ptr + 6));
        return(ptr + 8);
    }
    if (opc == 96) { //StoppeSoundSchleife.
        StopSoundLoop(game->soundManager);
        return(ptr + 2);
    }
    if (opc == 97) { //SoundVolPan.
//        SndVolPan(peekv(game, ptr + 2), peekv(game, ptr + 4));
        return(ptr + 6);
    }
    if (opc == 67) { //SpieleModule.
//        PlSpieleModule(peeks(script, ptr + 2), peekv(game, ptr + 6), peekv(game, ptr + 8));
        return(ptr + 10);
    }
    if (opc == 68) { //StoppeModule.
//        PlStoppeModule();
        return(ptr + 2);
    }
    if (opc == 69) { //ModuleWeiter.
//        PlModuleWeiter();
        return(ptr + 2);
    }
    if (opc == 65) { //ExternAmigaOS.
        return(ptr + 6);
    }
    if (opc == 49) { //ExternAmigaDE.
        return(ptr + 6);
    }
    if (opc == 71) { //ExternMacOS.
        return(ptr + 6);
    }
    if (opc == 98) { //ExternMorphOS.
        return(ptr + 6);
    }
    if (opc == 24) { //InventarNehmen.
        AddInventoryItem(game->gameState, peekv(game, ptr + 2), peeks(script, ptr + 4), peeks(script, ptr + 8), false);
        RefreshInventoryBar(game->inventoryBar, true);
        return(ptr + 12);
    }
    if (opc == 25) { //InventarWeg.
        RemoveInventoryItem(game->gameState, peekv(game, ptr + 2));
        RefreshInventoryBar(game->inventoryBar, true);
        return(ptr + 4);
    }
    if (opc == 20) { //WennGleich.
        if (peekv(game, ptr + 2) != peekv(game, ptr + 4)) {
            return(peekl(script, ptr + 6));
        } else {
            return(ptr + 10);
        }
    }
    if (opc == 21) { //WennAnders.
        if (peekv(game, ptr + 2) == peekv(game, ptr + 4)) {
            return(peekl(script, ptr + 6));
        } else {
            return(ptr + 10);
        }
    }
    if (opc == 50) { //WennGrößer.
        if (peekv(game, ptr + 2) <= peekv(game, ptr + 4)) {
            return(peekl(script, ptr + 6));
        } else {
            return(ptr + 10);
        }
    }
    if (opc == 51) { //WennKleiner.
        if (peekv(game, ptr + 2) >= peekv(game, ptr + 4)) {
            return(peekl(script, ptr + 6));
        } else {
            return(ptr + 10);
        }
    }
    if (opc == 45) { //WennSichtbar.
        if (GetVisibility(game->gameState, peekv(game, ptr + 2), peekv(game, ptr + 4))) {
            return(ptr + 10);
        } else {
            return(peekl(script, ptr + 6));
        }
    }
    if (opc == 46) { //WennUnsichtbar.
        if (GetVisibility(game->gameState, peekv(game, ptr + 2), peekv(game, ptr + 4))) {
            return(peekl(script, ptr + 6));
        } else {
            return(ptr + 10);
        }
    }
    if (opc == 35) { //SetzeVariable.
        SetVariable(game->gameState, peekv(game, ptr + 2), peekv(game, ptr + 4), false);
        return(ptr + 6);
    }
    if (opc == 36) { //InitVariable.
        SetVariable(game->gameState, peekv(game, ptr + 2), peekv(game, ptr + 4), true);
        return(ptr + 6);
    }
    if (opc == 60) { //HolePersVars.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (peekv(game, ptr + 4) > 0) SetVariable(game->gameState, peekv(game, ptr + 4), element->position.x, false);
        if (peekv(game, ptr + 6) > 0) SetVariable(game->gameState, peekv(game, ptr + 6), element->position.y, false);
        if (peekv(game, ptr + 8) > 0) SetVariable(game->gameState, peekv(game, ptr + 8), ImageSideFront, false); //TODO: direction
        if (peekv(game, ptr + 10) > 0) SetVariable(game->gameState, peekv(game, ptr + 10), 0, false); //TODO: action
        return(ptr + 12);
    }
    if (opc == 52) { //AddVariable.
        int varId = peekv(game, ptr + 2);
        SetVariable(game->gameState, varId, GetVariable(game->gameState, varId) + peekv(game, ptr + 4), false);
        return(ptr + 6);
    }
    if (opc == 53) { //SubVariable.
        int varId = peekv(game, ptr + 2);
        SetVariable(game->gameState, varId, GetVariable(game->gameState, varId) - peekv(game, ptr + 4), false);
        return(ptr + 6);
    }
    if (opc == 54) { //MulVariable.
        int varId = peekv(game, ptr + 2);
        SetVariable(game->gameState, varId, GetVariable(game->gameState, varId) * peekv(game, ptr + 4), false);
        return(ptr + 6);
    }
    if (opc == 55) { //DivVariable.
        int varId = peekv(game, ptr + 2);
        SetVariable(game->gameState, varId, GetVariable(game->gameState, varId) / peekv(game, ptr + 4), false);
        return(ptr + 6);
    }
    if (opc == 85) { //Zähle.
        int varId = peekv(game, ptr + 2);
        int value = GetVariable(game->gameState, varId) + 1;
        if (value > peekv(game, ptr + 6)) {
            value = peekv(game, ptr + 4);
        }
        SetVariable(game->gameState, varId, value, false);
        return(ptr + 8);
    }
    if (opc == 91) { //DivRestVariable.
        int varId = peekv(game, ptr + 2);
        SetVariable(game->gameState, varId, GetVariable(game->gameState, varId) % peekv(game, ptr + 4), false);
        return(ptr + 6);
    }
    if (opc == 38) { //Antwort.
        if (peekv(game, ptr + 8) == peekv(game, ptr + 10)) {
            AddDialogItem(game->dialog, peekv(game, ptr + 2), peeks(script, ptr + 4), game->font);
        }
        return(ptr + 12);
    }
    if (opc == 78) { //Leistensperre.
        game->inventoryBar->isEnabled = false;
        game->inventoryBar->isVisible = false;
        return(ptr + 2);
    }
    if (opc == 79) { //LeistensperreAus.
        game->inventoryBar->isEnabled = true;
        return(ptr + 2);
    }
    if (opc == 80) { //Menü.
//        zmenu = true;
        return(ptr + 2);
    }
    if (opc == 81) { //SpielEnde.
//        FadeOut(4);
//        SetShouldQuit();
        SetGameState(game, CreateGameState());
        *wieder = false;
        return thread->ptr;
    }
    if (opc == 89) { //HoleZeit.
//        if (peekv(game, ptr + 2) > 0) SetzeVar(peekv(game, ptr + 2), (WORD)zeit.stunden);
//        if (peekv(game, ptr + 4) > 0) SetzeVar(peekv(game, ptr + 4), (WORD)zeit.sekunden);
        return(ptr + 6);
    }
    if (opc == 90) { //ZeitDiff.
//        use = (peekv(game, ptr + 2) * 3600) + peekv(game, ptr + 4);
//        use = ((zeit.stunden * 3600) + (WORD)zeit.sekunden) - use;
//        SetzeVar(peekv(game, ptr + 6), (WORD)(use / 3600));
//        SetzeVar(peekv(game, ptr + 8), (WORD)(use % 3600));
        return(ptr + 10);
    }
    if (opc == 100) { //print.
//        if (devmodus)
        printf("%s\n", peeks(script, ptr + 2));
        return(ptr + 6);
    }
    if (opc == 101) { //printn.
//        if (devmodus)
        printf("%d\n", peekv(game, ptr + 2));
        return(ptr + 4);
    }

    printf("PC: %ld Opc: %d\n", ptr, opc);
    thread->isActive = false;
//    Fehler(6, "Unbekannter Skriptbefehl");
    return(ptr);
}

void EscapeThread(Thread *thread) {
    if (!thread || !thread->escptr) return;
    if (thread->talkingElement) {
        ElementStop(thread->talkingElement);
        thread->talkingElement = NULL;
    }
    thread->ptr = thread->escptr;
    thread->escptr = 0;
}

void RunThread(Thread *thread, unsigned long ptr) {
    if (!thread) return;
    thread->ptr = ptr;
    thread->isActive = true;
}

void StartInteraction(Thread *thread, int id1, int id2, Verb verb) {
    if (!thread) return;
    
    thread->angesehen = 0;
    thread->gesagt = 0;
    thread->benutzt = 0;
    thread->invbenutzt = 0;
    
    switch (verb) {
        case VerbUse:
            thread->benutzt = id1;
            thread->invbenutzt = id2;
            thread->ptr = thread->listeptr;
            break;
        case VerbLook:
            thread->angesehen = id1;
            thread->ptr = thread->listeptr;
            break;
        case VerbSay:
            thread->gesagt = id1;
            thread->ptr = thread->dialoglisteptr;
            break;
    }
    thread->isActive = true;
    HideCursor();
}

unsigned short peekv(Game *game, unsigned long pointer) {
    unsigned short value = peekw(game->script, pointer);
    if (value > 32767) {
        return GetVariable(game->gameState, value - 32768);
    }
    return value;
}
