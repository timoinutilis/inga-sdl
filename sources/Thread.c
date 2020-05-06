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

#include "Thread.h"
#include "Game.h"

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
    Script *script = game->script;
    unsigned short opc;
    unsigned long use;

    opc = peekv(game, ptr);
    *wieder = true;

    /*==Einrichtung==*/
    if (opc == 1) { //Einrichtung.
        SetLocation(game, peekv(game, ptr + 2), peeks(script, ptr + 4));
//        SndSchleifeAbbruch();
//        FadeOut(8);
//        MeldungAbbruch();
//        ort.id=peekv(game, ptr + 2);
//        if (peekv(game, ptr + 8) > 0) SpieleCDTrack(peekv(game, ptr + 8)); else StoppeCD();
//        person=SucheIDPerson(0); person->standiannum = 1;
//        iannumgehen = 2; hauptsichtbar = true;
//        invbar.sperre = FALSE; invbar.aktiv = FALSE;
//        modus = 0;
        return(ptr + 10);
    }
    if (opc == 2) { //EinrichtungEnde.
//        AktualisiereSichtbarkeit();
//        Restauration(); BltZierden(); BltTesteObjekte(); SortierePersonen(); BltTestePersonen(); BildWechsel();
//        MausStatusSichtbar(FALSE);
//        modus = 1; FadeIn(8);
        return(ptr + 2);
    }
    if (opc == 83) { //LadeBild.
//        LadeHintergrund(peeks(script, ptr + 2));
        return(ptr + 6);
    }
    if (opc == 3) { //Feld.
//        SichtInit(ort.id, peekv(game, ptr + 2), (bool)peekv(game, ptr + 20));
        Element *element = CreateElement(peekv(game, ptr + 2));
        element->selectionRect = MakeRectFromTo(peekv(game, ptr + 8), peekv(game, ptr + 10), peekv(game, ptr + 12), peekv(game, ptr + 14));
        strcpy(element->name, peeks(script, ptr + 4));
        element->target = MakeVector(peekv(game, ptr + 16), peekv(game, ptr + 18));
        element->isSelectable = true;
        AddElement(game->location, element);
        return(ptr + 22);
    }
    if (opc == 4) { //FeldUnsichtbar.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        element->isVisible = false;
        return(ptr + 4);
    }
    if (opc == 5) { //Zierde.
//        SichtInit(ort.id, peekv(game, ptr + 2), (bool)peekv(game, ptr + 12));
        Element *element = CreateElement(peekv(game, ptr + 2));
        element->position = MakeVector(peekv(game, ptr + 8), peekv(game, ptr + 10));
        element->image = LoadImage(peeks(script, ptr + 4), game->location->image->surface->format->palette, false, false);
        AddElement(game->location, element);
        return(ptr + 14);
    }
    if (opc == 6) { //ZierdeUnsichtbar.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        element->isVisible = false;
        return(ptr + 4);
    }
    if (opc == 7) { //Objekt.
//        SichtInit(ort.id, peekv(game, ptr + 2), (bool)peekv(game, ptr + 20));
        Element *element = CreateElement(peekv(game, ptr + 2));
        element->position = MakeVector(peekv(game, ptr + 12), peekv(game, ptr + 14));
        element->image = LoadImage(peeks(script, ptr + 8), game->location->image->surface->format->palette, false, false);
        strcpy(element->name, peeks(script, ptr + 4));
        element->target = MakeVector(peekv(game, ptr + 16), peekv(game, ptr + 18));
        element->isSelectable = true;
        AddElement(game->location, element);
        return(ptr + 22);
    }
    if (opc == 8) { //ObjektUnsichtbar.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        element->isVisible = false;
        return(ptr + 4);
    }
    if (opc == 62) { //ObjektStandbild.
//        ObjektStandbild(peekv(game, ptr + 2), peekv(game, ptr + 4));
        return(ptr + 6);
    }
    if (opc == 63) { //ObjektStandbildAus.
//        ObjektStandbild(peekv(game, ptr + 2), -1);
        return(ptr + 4);
    }
    if (opc == 9) { //Person.
//        SichtInit(ort.id, peekv(game, ptr + 2), (bool)peekv(game, ptr + 16));
        Element *element = CreateElement(peekv(game, ptr + 2));
        element->position = MakeVector(peekv(game, ptr + 12), peekv(game, ptr + 14));
        element->imageSet = LoadImageSet(peeks(script, ptr + 8), game->location->image->surface->format->palette, true);
        strcpy(element->name, peeks(script, ptr + 4));
        element->isSelectable = true;
        AddElement(game->location, element);
        return(ptr + 18);
    }
    if (opc == 82) { //PersonProg.
//        person=SucheIDPerson(peekv(game, ptr + 2));
//        if (person) {
//            person->pc = ptr + 8;
//            person->isfaktiv = true;
//        }
        return(peekl(script, ptr + 4));
    }
    if (opc == 10) { //PersonUnsichtbar.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        element->isVisible = false;
        return(ptr + 4);
    }
    if (opc == 93) { //PersonVgMaskeAktiv
//        person = SucheIDPerson(peekv(game, ptr + 2));
//        if (person) person->vgmaske = true;
        return(ptr + 4);
    }
    if (opc == 94) { //PersonVgMaskeInaktiv
//        person = SucheIDPerson(peekv(game, ptr + 2));
//        if (person) person->vgmaske = FALSE;
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
        if (game->location->id == peekv(game, ptr + 2)) {
            Element *element = GetElement(game->location, peekv(game, ptr + 4));
            element->isVisible = true;
        }
//        if (peekv(game, ptr + 4) == 0) {
//            hauptsichtbar = true
//        } else {
//            SetzeSicht(peekv(game, ptr + 2), peekv(game, ptr + 4), true);
//        }
//        AktualisiereSichtbarkeit();
        return(ptr + 6);
    }
    if (opc == 18) { //Unsichtbar.
        if (game->location->id == peekv(game, ptr + 2)) {
            Element *element = GetElement(game->location, peekv(game, ptr + 4));
            element->isVisible = false;
        }
//        if (peekv(game, ptr + 4) == 0) {
//            hauptsichtbar = FALSE
//        } else {
//            SetzeSicht(peekv(game, ptr + 2), peekv(game, ptr + 4), FALSE);
//        }
//        AktualisiereSichtbarkeit();
        return(ptr + 6);
    }
    if (opc == 72) { //ilkOben.
//        LaufkarteLinie(0, peekv(game, ptr + 2), peekv(game, ptr + 4), peekv(game, ptr + 6), peekv(game, ptr + 8));
        return(ptr + 10);
    }
    if (opc == 73) { //ilkUnten.
//        LaufkarteLinie(1, peekv(game, ptr + 2), peekv(game, ptr + 4), peekv(game, ptr + 6), peekv(game, ptr + 8));
        return(ptr + 10);
    }
    if (opc == 74) { //LadeLaufkarte.
//        LadeLaufkarte(peeks(script, ptr + 2));
        return(ptr + 6);
    }
    
    /* == Personen == */
    if (opc == 70) { //Lauftempo.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element->action == ElementActionIdle) {
            int factor = peekv(game, ptr + 4);
            if (factor < 1) {
                factor = 1;
            }
            element->speed = factor * 20; // pxl/s
            return(ptr + 6);
        }
        *wieder = false;
        return(ptr);
    }
    if (opc == 11) { //Laufe.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element->action == ElementActionIdle) {
            ElementMoveTo(element, peekv(game, ptr + 4), peekv(game, ptr + 6), peekv(game, ptr + 8));
            return(ptr + 10);
        }
        *wieder = false;
        return(ptr);
    }
    if (opc == 12) { //LaufeDirekt.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element->action == ElementActionIdle) {
            //TODO: skip navigation
            ElementMoveTo(element, peekv(game, ptr + 4), peekv(game, ptr + 6), peekv(game, ptr + 8));
            return(ptr + 10);
        }
        *wieder = false;
        return(ptr);
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
        if (element->action == ElementActionIdle) {
            ElementTalk(element, peeks(script, ptr + 6), peekv(game, ptr + 4), game->font);
            thread->talkingElement = element;
            // sound: peeks(script, ptr + 10)
            return(ptr + 14);
        }
        *wieder = false;
        return(ptr);
    }
    if (opc == 14) { //Anim.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element->action == ElementActionIdle) {
            ElementAnimate(element, peekv(game, ptr + 4), peekv(game, ptr + 6));
            return(ptr + 8);
        }
        *wieder = false;
        return(ptr);
    }
    if (opc == 41) { //AnimNehmen.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element->action == ElementActionIdle) {
            ElementAnimate(element, peekv(game, ptr + 6), 1);
            //TODO: hide object
//            PersonenAktion(peekv(game, ptr + 2), AKT_NEHMEN, peekv(game, ptr + 4), peekv(game, ptr + 6), peekv(game, ptr + 8), 0);
            return(ptr + 10);
        }
        *wieder = false;
        return(ptr);
    }
    if (opc == 84) { //NichtsProg.
//        nichtsprog.frame = 0;
//        nichtsprog.dauer = peekv(game, ptr + 2);
//        nichtsprog.ptr = peekl(script, ptr + 4);
        return(ptr + 8);
    }
    if (opc == 86) { //LöscheNichtsProg.
//        nichtsprog.frame = 0;
//        nichtsprog.dauer = 0;
//        nichtsprog.ptr = 0;
        return(ptr + 2);
    }
    if (opc == 15) { //Richtung.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element->action == ElementActionIdle) {
            ElementSetSide(element, peekv(game, ptr + 4), 0);
            return(ptr + 6);
        }
        *wieder = false;
        return(ptr);
    }
    if (opc == 32) { //RichteAuf.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element->action == ElementActionIdle) {
            Element *targetElement = GetElement(game->location, peekv(game, ptr + 4));
            ElementLookTo(element, targetElement->position.x, targetElement->position.y, 0);
            return(ptr + 6);
        }
        *wieder = false;
        return(ptr);
    }
    if (opc == 33) { //RichteAufeinander.
        Element *element1 = GetElement(game->location, peekv(game, ptr + 2));
        if (element1->action == ElementActionIdle) {
            Element *element2 = GetElement(game->location, peekv(game, ptr + 4));
            if (element2->action == ElementActionIdle) {
                ElementLookTo(element1, element2->position.x, element2->position.y, 0);
                ElementLookTo(element2, element1->position.x, element1->position.y, 0);
                return(ptr + 6);
            }
        }
        *wieder = false;
        return(ptr);
    }
    if (opc == 16) { //StelleAuf.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element->action == ElementActionIdle) {
            element->position.x = peekv(game, ptr + 4);
            element->position.y = peekv(game, ptr + 6);
            ElementSetSide(element, peekv(game, ptr + 8), 0);
            return(ptr + 10);
        }
        *wieder = false;
        return(ptr);
    }
    if (opc == 17) { //WarteAuf.
        Element *element = GetElement(game->location, peekv(game, ptr + 2));
        if (element->action == ElementActionIdle) {
            return(ptr + 4);
        }
        *wieder = false;
        return(ptr);
    }
    if (opc == 37) { //WarteAufAnim.
//        person = SucheIDPerson(peekv(game, ptr + 2));
//        if (person->ian->frames >= peekv(game, ptr + 4)) {
//            if (person->animnum >= peekv(game, ptr + 4)) {
                return(ptr + 6);
//            } else return(ptr);
//        } else return(ptr + 6);
    }
    if (opc == 30) { //Aktiv.
//        person = SucheIDPerson(peekv(game, ptr + 2));
//        if (person->pc > 0) person->isfaktiv = true;
        return(ptr + 4);
    }
    if (opc == 31) { //Inaktiv.
//        person = SucheIDPerson(peekv(game, ptr + 2));
//        person->aktion = AKT_NICHTS;
//        person->isfaktiv = FALSE;
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
//        person = SucheIDPerson(0);
//        person->standiannum = peekv(game, ptr + 2);
//        iannumgehen = peekv(game, ptr + 4);
        return(ptr + 6);
    }
    if (opc == 87) { //StandAnim.
//        person = SucheIDPerson(peekv(game, ptr + 2));
//        person->standiannum = peekv(game, ptr + 4);
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
        game->mainPerson->position = MakeVector(peekv(game, ptr + 6), peekv(game, ptr + 8));
        ElementSetSide(game->mainPerson, peekv(game, ptr + 10), 0);
//        ort.ptr = peekl(script, ptr + 2);
        return(peekl(script, ptr + 2));
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
//        MausStatusSichtbar(true);
        thread->benutzt = 0;
        thread->invbenutzt = 0;
        thread->angesehen = 0;
        thread->gesagt = 0;
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
//        MausStatusSichtbar(true);
        thread->listeptr = ptr + 2;
        thread->isActive = false;
        return(ptr + 2);
    }
    if (opc == 39) { //Dialogliste.
//        MausStatusSichtbar(true);
        thread->dialoglisteptr = ptr + 2;
//        dialog.aktiv = true;
        thread->isActive = false;
        return(ptr + 2);
    }
    if (opc == 26) { //WennBenutzt.
        if ((thread->benutzt == 0) || (thread->invbenutzt > 0)) return(peekl(script, ptr + 4));
        if ((peekv(game, ptr + 2) == thread->benutzt) || (peekv(game, ptr + 2) == 0)) {
            return(ptr + 8);
        } else {
            return(peekl(script, ptr + 4));
        }
    }
    if (opc == 27) { //WennBenutztMit.
        if ((thread->benutzt > 0) && (thread->invbenutzt > 0)) {
            if (((peekv(game, ptr + 2) == thread->benutzt) && (peekv(game, ptr + 4) == thread->invbenutzt)) ||
                ((peekv(game, ptr + 2) == thread->invbenutzt) && (peekv(game, ptr + 4) == thread->benutzt))) {
//                benutzt = 0; invbenutzt = 0; modus = 1;
                return(ptr + 10);
            }
            if ((peekv(game, ptr + 2) == 0) && ((peekv(game, ptr + 4) == thread->benutzt) || (peekv(game, ptr + 4) == thread->invbenutzt))) {
//                benutzt = 0; invbenutzt = 0; modus = 1;
                return(ptr + 10);
            }
            if ((peekv(game, ptr + 2) == 0) && (peekv(game, ptr + 4) == 0)) {
//                benutzt = 0; invbenutzt = 0; modus = 1;
                return(ptr + 10);
            }
        }
        return(peekl(script, ptr + 6));
    }
    if (opc == 28) { //WennAngesehen.
        if (thread->angesehen == 0) return(peekl(script, ptr + 4));
        if ((peekv(game, ptr + 2) == thread->angesehen) || (peekv(game, ptr + 2) == 0)) {
//            angesehen = 0; modus = 1;
            return(ptr + 8);
        } else {
            return(peekl(script, ptr + 4));
        }
    }
    if (opc == 40) { //WennGesagt.
        if (peekv(game, ptr + 2) == thread->gesagt) {
//            gesagt = 0; modus = 1;
            return(ptr + 8);
        } else {
            return(peekl(script, ptr + 4));
        }
    }
    if (opc == 34) { //Sequenz.
//        SndSchleifeAbbruch();
//        Sequenz(peeks(script, ptr + 2));
        return(ptr + 6);
    }
    if (opc == 47) { //CDNummer.
//        SpieleCDTrack(peekv(game, ptr + 2));
        return(ptr + 4);
    }
    if (opc == 48) { //CDStopp.
//        StoppeCD();
        return(ptr + 2);
    }
    if (opc == 61) { //LadeSound.
//        SndLadeSound(peeks(script, ptr + 4), peekv(game, ptr + 2));
        return(ptr + 8);
    }
    if (opc == 64) { //EntferneSound.
//        SndEntferneSound(peekv(game, ptr + 2));
        return(ptr + 4);
    }
    if (opc == 66) { //SpieleSound.
//        SndSpieleSound(peekv(game, ptr + 2), peekv(game, ptr + 4), peekv(game, ptr + 6));
        return(ptr + 8);
    }
    if (opc == 95) { //SpieleSoundSchleife.
//        SndSpieleSoundSchleife(peekv(game, ptr + 2), peekv(game, ptr + 4), peekv(game, ptr + 6));
        return(ptr + 8);
    }
    if (opc == 96) { //StoppeSoundSchleife.
//        SndSchleifeAbbruch();
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
//        Execute(peeks(script, ptr + 2), NULL, NULL);
//        ScreenToFront(schirm);
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
//        AddInventar(peeks(script, ptr + 8), peeks(script, ptr + 4), peekv(game, ptr + 2));
        return(ptr + 12);
    }
    if (opc == 25) { //InventarWeg.
//        EntferneInventar(peekv(game, ptr + 2));
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
//        if (SichtWert(peekv(game, ptr + 2), peekv(game, ptr + 4)))
//            return(ptr + 10);
//        else
            return(peekl(script, ptr + 6));
    }
    if (opc == 46) { //WennUnsichtbar.
//        if (SichtWert(peekv(game, ptr + 2), peekv(game, ptr + 4)))
//            return(peekl(script, ptr + 6));
//        else
            return(ptr + 10);
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
//        person = SucheIDPerson(peekv(game, ptr + 2));
//        if (peekv(game, ptr + 4) > 0) SetzeVar(peekv(game, ptr + 4), (WORD)person->x);
//        if (peekv(game, ptr + 6) > 0) SetzeVar(peekv(game, ptr + 6), (WORD)person->y);
//        if (peekv(game, ptr + 8) > 0) SetzeVar(peekv(game, ptr + 8), person->richtung);
//        if (peekv(game, ptr + 10) > 0) SetzeVar(peekv(game, ptr + 10), person->aktion);
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
//        if (peekv(game, ptr + 8) == peekv(game, ptr + 10)) AddAntwort(peekv(game, ptr + 2), peeks(script, ptr + 4));
        return(ptr + 12);
    }
    if (opc == 78) { //Leistensperre.
//        invbar.sperre = true;
//        if (invbar.aktiv) InvBarWeg();
        return(ptr + 2);
    }
    if (opc == 79) { //LeistensperreAus.
//        invbar.sperre = FALSE;
        return(ptr + 2);
    }
    if (opc == 80) { //Menü.
//        zmenu = true;
        return(ptr + 2);
    }
    if (opc == 81) { //SpielEnde.
//        FadeOut(4); Ende();
        exit(0);
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

void StartInteraction(Thread *thread, int id, Verb verb) {
    switch (verb) {
        case VerbUse:
            thread->benutzt = id;
            break;
        case VerbLook:
            thread->angesehen = id;
            break;
    }
    thread->ptr = thread->listeptr;
    thread->isActive = true;
}

unsigned short peekv(Game *game, unsigned long pointer) {
    unsigned short value = peekw(game->script, pointer);
    if (value > 32767) {
        return GetVariable(game->gameState, value - 32768);
    }
    return value;
}
