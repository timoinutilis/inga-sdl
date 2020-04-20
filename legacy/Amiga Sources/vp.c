#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dos/dos.h>
#include <intuition/intuition.h>
#include <devices/timer.h>

#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/timer.h>

#include "strukturen.h"
#include "vp.h"
#include "elem_felder.h"
#include "elem_zierden.h"
#include "elem_objekte.h"
#include "elem_personen.h"
#include "grafik.h"
#include "inventar.h"
#include "dialog.h"
#include "textausgabe.h"
#include "ingasound.h"
#include "ingaplayer.h"
#include "ingaaudiocd.h"
#include "kernel.h"

void Sequenz(STRPTR datei);

//Systemzeiger
extern struct Screen *schirm;

//Programmsystemvariablen
extern struct timeval systime;
extern UBYTE modus;
extern BOOL hauptsichtbar;
extern UWORD iannumgehen;
extern UBYTE sysfar[6];
extern BOOL zmenu;
extern BOOL devmodus;

//VP-Variablen
APTR inga = NULL;
APTR itxt = NULL;
ULONG listeptr = 0;
ULONG dialoglisteptr = 0;
ULONG subptr = 0;
ULONG escptr = 0;

//Elementzeiger
extern struct FELD *rootfeld;
extern struct ZIERDE *rootzierde;
extern struct OBJEKT *rootobjekt;
extern struct PERSON *rootperson;
struct VARIABLE *rootvar = NULL;
struct SICHT *rootsicht = NULL;

//Datenstrukturen
extern struct ORT ort;
extern struct SOUNDBASE soundbase;
extern struct DIALOG dialog;
extern struct SPRECH sprech;
extern struct INVBAR invbar;
extern struct NICHTSPROG nichtsprog;
extern struct SPIELZEIT zeit;

//Aktionen
extern UWORD benutzt;
extern UWORD invbenutzt;
extern UWORD angesehen;
extern UBYTE gesagt;

 
/*=========================================*/
void AktualisiereSichtbarkeit() {
	struct FELD *feld=rootfeld;
	struct ZIERDE *zierde=rootzierde;
	struct OBJEKT *objekt=rootobjekt;
	struct PERSON *person=rootperson;

	while (feld) {
		feld->sichtbar = SichtWert(ort.id, feld->id);
		feld = feld->next;
	}
	while (zierde) {
		zierde->sichtbar = SichtWert(ort.id, zierde->id);
		zierde = zierde->next;
	}
	while (objekt) {
		objekt->sichtbar = SichtWert(ort.id, objekt->id);
		objekt = objekt->next;
	}
	while (person) {
		if (person->id == 0) {
			person->sichtbar = hauptsichtbar;
		} else {
			person->sichtbar = SichtWert(ort.id, person->id);
		}
		person = person->next;
	}
}

void LadeINGA() {
	BPTR file;
	LONG len;

	MausStatusWarte(TRUE);
	if (file = Open("Dats/story.inga", MODE_OLDFILE)) {
		Seek(file, 0, OFFSET_END); len = Seek(file, 0, OFFSET_BEGINNING);
		if (inga = malloc(len)) {
			Read(file, inga, len);
		} else Fehler(0, "story.inga");
		Close(file);
	} else Fehler(2, "story.inga");
	
	if (file = Open("Dats/story.itxt", MODE_OLDFILE)) {
		Seek(file, 0, OFFSET_END); len = Seek(file, 0, OFFSET_BEGINNING);
		if (itxt = malloc(len)) {
			Read(file, itxt, len);
		} else Fehler(0, "story.itxt");
		Close(file);
	} else Fehler(2, "story.itxt");
	MausStatusWarte(FALSE);
}

void EntferneINGA() {
	if (inga) free(inga);
	if (itxt) free(itxt);
}

// Liest ein WORD aus dem Byte-Code (ggf. Variablenwert).
UWORD peekw(ULONG zeiger) {
	UWORD *ptr;
	ptr = (UWORD *)((ULONG)inga + zeiger);
	if (*ptr > 32767) return(VarWert(*ptr - 32768)); else return(*ptr);
}

// Liest ein LONG aus dem Byte-Code.
ULONG peekl(ULONG zeiger) {
	ULONG *ptr;
	ptr = (ULONG *)((ULONG)inga + zeiger);
	return(*ptr);
}

// Liest die Adresse einer Zeichenkette aus dem Byte-Code.
STRPTR peeks(ULONG zeiger) {
	STRPTR ptr;
	ptr = (STRPTR)((ULONG)itxt + peekl(zeiger));
	return(ptr);
}

// Führt einen Befehl aus dem Byte-Code aus.
ULONG LaufeINGA(ULONG ptr, BOOL *wieder) {
	UWORD opc;
	ULONG use;
	struct PERSON *person;

	opc = peekw(ptr);
	*wieder = FALSE;

	/*==Einrichtung==*/
	if (opc == 1) { //Einrichtung.
		StoppeReden();
		SndSchleifeAbbruch();
		FadeOut(8);
		MeldungAbbruch();
		EntferneAllePersonen(FALSE);
		EntferneAlleZierden();
		EntferneAlleObjekte();
		EntferneAlleFelder();
		ort.id=peekw(ptr + 2);
		if (peekw(ptr + 8) > 0) SpieleCDTrack(peekw(ptr + 8)); else StoppeCD();
		LadeHintergrund(peeks(ptr + 4));
		LadeLaufkarte(peeks(ptr + 4));
		person=SucheIDPerson(0); person->standiannum = 1;
		iannumgehen = 2; hauptsichtbar = TRUE;
		invbar.sperre = FALSE; invbar.aktiv = FALSE;
		modus = 0; return(ptr + 10);
	}
	if (opc == 2) { //EinrichtungEnde.
		AktualisiereSichtbarkeit();
		Restauration(); BltZierden(); BltTesteObjekte(); SortierePersonen(); BltTestePersonen(); BildWechsel();
		MausStatusSichtbar(FALSE);
		modus = 1; FadeIn(8); return(ptr + 2);
	}
	if (opc == 83) { //LadeBild.
		LadeHintergrund(peeks(ptr + 2));
		return(ptr + 6);
	}
	if (opc == 3) { //Feld.
		SichtInit(ort.id, peekw(ptr + 2), (BOOL)peekw(ptr + 20));
		AddFeld(peekw(ptr + 2), peekw(ptr + 8), peekw(ptr + 10), peekw(ptr + 12), peekw(ptr + 14), peeks(ptr + 4), peekw(ptr + 16), peekw(ptr + 18));
		return(ptr + 22);
	}
	if (opc == 4) { //FeldUnsichtbar.
		EntferneFeld(peekw(ptr + 2)); return(ptr + 4);
	}
	if (opc == 5) { //Zierde.
		SichtInit(ort.id, peekw(ptr + 2), (BOOL)peekw(ptr + 12));
		AddZierde(peekw(ptr + 2), peekw(ptr + 8), peekw(ptr + 10), peeks(ptr + 4));
		return(ptr + 14);
	}
	if (opc == 6) { //ZierdeUnsichtbar.
		EntferneZierde(peekw(ptr + 2)); return(ptr + 4);
	}
	if (opc == 7) { //Objekt.
		SichtInit(ort.id, peekw(ptr + 2), (BOOL)peekw(ptr + 20));
		AddObjekt(peekw(ptr + 2), peekw(ptr + 12), peekw(ptr + 14), peeks(ptr + 8), peeks(ptr + 4), peekw(ptr + 16), peekw(ptr + 18));
		return(ptr + 22);
	}
	if (opc == 8) { //ObjektUnsichtbar.
		EntferneObjekt(peekw(ptr + 2));
		return(ptr + 4);
	}
	if (opc == 62) { //ObjektStandbild.
		ObjektStandbild(peekw(ptr + 2), peekw(ptr + 4));
		return(ptr + 6);
	}
	if (opc == 63) { //ObjektStandbildAus.
		ObjektStandbild(peekw(ptr + 2), -1);
		return(ptr + 4);
	}
	if (opc == 9) { //Person.
		SichtInit(ort.id, peekw(ptr + 2), (BOOL)peekw(ptr + 16));
		AddPerson(peekw(ptr + 2), peekw(ptr + 12), peekw(ptr + 14), peeks(ptr + 8), peeks(ptr + 4));
		return(ptr + 18);
	}
	if (opc == 82) { //PersonProg.
		person=SucheIDPerson(peekw(ptr + 2));
		if (person) {
			person->pc = ptr + 8;
			person->isfaktiv = TRUE;
		}
		return(peekl(ptr + 4));
	}
	if (opc == 10) { //PersonUnsichtbar.
		EntfernePerson(peekw(ptr + 2)); return(ptr + 4);
	}
	if (opc == 93) { //PersonVgMaskeAktiv
		person = SucheIDPerson(peekw(ptr + 2));
		if (person) person->vgmaske = TRUE;
		return(ptr + 4);
	}
	if (opc == 94) { //PersonVgMaskeInaktiv
		person = SucheIDPerson(peekw(ptr + 2));
		if (person) person->vgmaske = FALSE;
		return(ptr + 4);
	}
	if (opc == 76) { //Vorladen.
		Vorladen(peekw(ptr + 2), peekw(ptr + 4));
		return(ptr + 6);
	}
	if (opc == 77) { //Freigeben.
		Freigeben(peekw(ptr + 2), peekw(ptr + 4));
		return(ptr + 6);
	}
	if (opc == 42) { //Sichtbar.
		if (peekw(ptr + 4) == 0) {
			hauptsichtbar = TRUE
		} else {
			SetzeSicht(peekw(ptr + 2), peekw(ptr + 4), TRUE);
		}
		AktualisiereSichtbarkeit();
		return(ptr + 6);
	}
	if (opc == 18) { //Unsichtbar.
		if (peekw(ptr + 4) == 0) {
			hauptsichtbar = FALSE
		} else {
			SetzeSicht(peekw(ptr + 2), peekw(ptr + 4), FALSE);
		}
		AktualisiereSichtbarkeit();
		return(ptr + 6);
	}
	if (opc == 72) { //ilkOben.
		LaufkarteLinie(0, peekw(ptr + 2), peekw(ptr + 4), peekw(ptr + 6), peekw(ptr + 8));
		return(ptr + 10);
	}
	if (opc == 73) { //ilkUnten.
		LaufkarteLinie(1, peekw(ptr + 2), peekw(ptr + 4), peekw(ptr + 6), peekw(ptr + 8));
		return(ptr + 10);
	}
	if (opc == 74) { //LadeLaufkarte.
		LadeLaufkarte(peeks(ptr + 2));
		return(ptr + 6);
	}
	
	/* == Personen == */
	if (opc == 70) { //Lauftempo.
		person = SucheIDPerson(peekw(ptr + 2));
		if (person->aktion == AKT_NICHTS) {
			person->lauffaktor = (UBYTE)peekw(ptr + 4);
			if (person->lauffaktor < 1) person->lauffaktor = 1;
			return(ptr + 6);
		} else return(ptr);
	}
	if (opc == 11) { //Laufe.
		person = SucheIDPerson(peekw(ptr + 2));
		if (person->aktion == AKT_NICHTS) {
			PersonenAktion(peekw(ptr + 2), AKT_LAUFEN, peekw(ptr + 4), peekw(ptr + 6), peekw(ptr + 8), 0);
			return(ptr + 10);
		} else return(ptr);
	}
	if (opc == 12) { //LaufeDirekt.
		person = SucheIDPerson(peekw(ptr + 2));
		if (person->aktion == AKT_NICHTS) {
			PersonenAktion(peekw(ptr + 2), AKT_LAUFEN, peekw(ptr + 4), peekw(ptr + 6), peekw(ptr + 8), 1);
			return(ptr + 10);
		} else return(ptr);
	}
	if (opc == 13) { //Rede.
		person = SucheIDPerson(peekw(ptr + 2));
		if ((sprech.pers == -1) && (person->aktion == AKT_NICHTS)) {
			PersonenAktion(peekw(ptr + 2), AKT_REDEN, peekw(ptr + 4), 0, 0, 0);
			sprech.x = (WORD)person->x;
			sprech.y = (WORD)person->y - person->ian->greifpy - 10;
			sprech.pers = peekw(ptr + 2);
			strcpy(sprech.text, peeks(ptr + 6));
			if (sprech.spracheaktiv && soundbase.soundplugin && (strcmp(peeks(ptr + 10), "")!=0)) {
				sprech.sound = TRUE;
				soundbase.speech = TRUE;
				SndSage(peeks(ptr + 10), (WORD)person->x);
			} else {
				sprech.zeitz = (20 + (strlen(sprech.text) / 2)) * sprech.tempo;
			}
			return(ptr + 14);
		} else return(ptr);
	}
	if (opc == 14) { //Anim.
		person = SucheIDPerson(peekw(ptr + 2));
		if (person->aktion == AKT_NICHTS) {
			PersonenAktion(peekw(ptr + 2), AKT_ANIM, peekw(ptr + 4), peekw(ptr + 6), 0, 0);
			return(ptr + 8);
		} else return(ptr);
	}
	if (opc == 41) { //AnimNehmen.
		person = SucheIDPerson(peekw(ptr + 2));
		if (person->aktion == AKT_NICHTS) {
			PersonenAktion(peekw(ptr + 2), AKT_NEHMEN, peekw(ptr + 4), peekw(ptr + 6), peekw(ptr + 8), 0);
			return(ptr + 10);
		} else return(ptr);
	}
	if (opc == 84) { //NichtsProg.
		nichtsprog.frame = 0;
		nichtsprog.dauer = peekw(ptr + 2);
		nichtsprog.ptr = peekl(ptr + 4);
		return(ptr + 8);
	}
	if (opc == 86) { //LöscheNichtsProg.
		nichtsprog.frame = 0;
		nichtsprog.dauer = 0;
		nichtsprog.ptr = 0;
		return(ptr + 2);
	}
	if (opc == 15) { //Richtung.
		person = SucheIDPerson(peekw(ptr + 2));
		if (person->aktion == AKT_NICHTS) {
			person->richtung = peekw(ptr + 4);
			return(ptr + 6);
		} else return(ptr);
	}
	if (opc == 32) { //RichteAuf.
		person = SucheIDPerson(peekw(ptr + 2));
		if (person->aktion == AKT_NICHTS) {
			person = SucheIDPerson(peekw(ptr + 4));
			PersonRichtung(peekw(ptr + 2), (WORD)person->x, (WORD)person->y);
			return(ptr + 6);
		} else return(ptr);
	}
	if (opc == 33) { //RichteAufeinander.
		person = SucheIDPerson(peekw(ptr + 2));
		if (person->aktion == AKT_NICHTS) {
			person = SucheIDPerson(peekw(ptr + 4));
			if (person->aktion == AKT_NICHTS) {
				PersonRichtung(peekw(ptr + 2), (WORD)person->x, (WORD)person->y);
				person = SucheIDPerson(peekw(ptr + 2));
				PersonRichtung(peekw(ptr + 4), (WORD)person->x, (WORD)person->y);
				return(ptr + 6);
			}
		}
		return(ptr);
	}
	if (opc == 16) { //StelleAuf.
		person = SucheIDPerson(peekw(ptr + 2));
		if (person->aktion == AKT_NICHTS) {
			person->x = peekw(ptr + 4);
			person->y = peekw(ptr + 6);
			person->richtung = peekw(ptr + 8);
			return(ptr + 10);
		} else return(ptr);
	}
	if (opc == 17) { //WarteAuf.
		person = SucheIDPerson(peekw(ptr + 2));
		if (person->aktion == AKT_NICHTS) {
			return(ptr + 4);
		} else return(ptr);
	}
	if (opc == 37) { //WarteAufAnim.
		person = SucheIDPerson(peekw(ptr + 2));
		if (person->ian->frames >= peekw(ptr + 4)) {
			if (person->animnum >= peekw(ptr + 4)) {
				return(ptr + 6);
			} else return(ptr);
		} else return(ptr + 6);
	}
	if (opc == 30) { //Aktiv.
		person = SucheIDPerson(peekw(ptr + 2));
		if (person->pc > 0) person->isfaktiv = TRUE;
		return(ptr + 4);
	}
	if (opc == 31) { //Inaktiv.
		person = SucheIDPerson(peekw(ptr + 2));
		person->aktion = AKT_NICHTS;
		person->isfaktiv = FALSE; return(ptr + 4);
	}
	if (opc == 92) { //WechsleHauptIPE.
		person = SucheIDPerson(0);
		if (person->aktion == AKT_NICHTS) {
			ErsetzeHauptpersonIPE(peeks(ptr + 2));
		} else return(ptr);
		return(ptr + 6);
	}
	if (opc == 56) { //PersonenAnims.
		person = SucheIDPerson(0);
		person->standiannum = peekw(ptr + 2);
		iannumgehen = peekw(ptr + 4);
		return(ptr + 6);
	}
	if (opc == 87) { //StandAnim.
		person = SucheIDPerson(peekw(ptr + 2));
		person->standiannum = peekw(ptr + 4);
		return(ptr + 6);
	}

	/* == Systemsteuerung == */
	if (opc == 59) { //Farben.
		sysfar[0] = peekw(ptr + 2);
		sysfar[1] = peekw(ptr + 4);
		sysfar[2] = peekw(ptr + 6);
		sysfar[3] = peekw(ptr + 8);
		sysfar[4] = peekw(ptr + 10);
		sysfar[5] = peekw(ptr + 12);
		return(ptr + 14);
	}
	if (opc == 19) { //Springe.
		return(peekl(ptr + 2));
	}
	if (opc == 29) { //SpringeOrt.
		person = SucheIDPerson(0); modus = 0;
		person->x = peekw(ptr + 6);
		person->y = peekw(ptr + 8);
		person->richtung = peekw(ptr + 10);
		ort.ptr = peekl(ptr + 2);
		return(peekl(ptr + 2));
	}
	if (opc == 43) { //SpringeSub.
		if (subptr == 0) {
			subptr = ptr + 6;
			return(peekl(ptr + 2));
		} else Fehler(6, "Verschachteltes 'Sub'");
	}
	if (opc == 44) { //Zurück.
		if (subptr > 0) {
			use = subptr; subptr = 0;
			return(use);
		} else Fehler(6, "'Zurück' ohne 'Sub'");
	}
	if (opc == 57) { //SpringeEsc.
		escptr = peekl(ptr + 2);
		return(ptr + 6);
	}
	if (opc == 58) { //EscEnde.
		escptr = 0; return(ptr + 2);
	}
	if (opc == 22) { //Stopp.
		MausStatusSichtbar(TRUE);
		modus = 2; benutzt = 0; invbenutzt = 0; angesehen = 0; gesagt = 0; return(ptr + 2);
	}
	if (opc == 88) { //ListeAbbruch.
		modus = 1; benutzt = 0; invbenutzt = 0; angesehen = 0; gesagt = 0;
		return(ptr + 2);
	}
	if (opc == 23) { //Liste.
		MausStatusSichtbar(TRUE);
		listeptr = ptr + 2;
		modus = 2; return(ptr + 2);
	}
	if (opc == 39) { //Dialogliste.
		MausStatusSichtbar(TRUE);
		dialoglisteptr = ptr + 2; dialog.aktiv = TRUE;
		modus = 2; return(ptr + 2);
	}
	if (opc == 26) { //WennBenutzt.
		*wieder = TRUE;
		if ((benutzt == 0) || (invbenutzt > 0)) return(peekl(ptr + 4));
		if ((peekw(ptr + 2) == benutzt) || (peekw(ptr + 2) == 0)) {
			benutzt = 0; modus = 1; return(ptr + 8);
		} else return(peekl(ptr + 4));
	}
	if (opc == 27) { //WennBenutztMit.
		*wieder = TRUE;
		if ((benutzt > 0) && (invbenutzt > 0)) {
			if (((peekw(ptr + 2) == benutzt) && (peekw(ptr + 4) == invbenutzt)) ||
				((peekw(ptr + 2) == invbenutzt) && (peekw(ptr + 4) == benutzt))) {
				benutzt = 0; invbenutzt = 0; modus = 1; return(ptr + 10);
			}
			if ((peekw(ptr + 2) == 0) && ((peekw(ptr + 4) == benutzt) || (peekw(ptr + 4) == invbenutzt))) {
				benutzt = 0; invbenutzt = 0; modus = 1; return(ptr + 10);
			}
			if ((peekw(ptr + 2) == 0) && (peekw(ptr + 4) == 0)) {
				benutzt = 0; invbenutzt = 0; modus = 1; return(ptr + 10);
			}
		}
		return(peekl(ptr + 6));
	}
	if (opc == 28) { //WennAngesehen.
		*wieder = TRUE;
		if (angesehen == 0) return(peekl(ptr + 4));
		if ((peekw(ptr + 2) == angesehen) || (peekw(ptr + 2) == 0)) {
			angesehen = 0; modus = 1; return(ptr + 8);
		} else return(peekl(ptr + 4));
	}
	if (opc == 40) { //WennGesagt.
		*wieder = TRUE;
		if (peekw(ptr + 2) == gesagt) {
			gesagt = 0; modus = 1; return(ptr + 8);
		} else return(peekl(ptr + 4));
	}
	if (opc == 34) { //Sequenz.
		SndSchleifeAbbruch();
		Sequenz(peeks(ptr + 2));
		return(ptr + 6);
	}
	if (opc == 47) { //CDNummer.
		*wieder = TRUE;
		SpieleCDTrack(peekw(ptr + 2));
		return(ptr + 4);
	}
	if (opc == 48) { //CDStopp.
		*wieder = TRUE;
		StoppeCD();
		return(ptr + 2);
	}
	if (opc == 61) { //LadeSound.
		SndLadeSound(peeks(ptr + 4), peekw(ptr + 2));
		return(ptr + 8);
	}
	if (opc == 64) { //EntferneSound.
		SndEntferneSound(peekw(ptr + 2));
		return(ptr + 4);
	}
	if (opc == 66) { //SpieleSound.
		SndSpieleSound(peekw(ptr + 2), peekw(ptr + 4), peekw(ptr + 6));
		return(ptr + 8);
	}
	if (opc == 95) { //SpieleSoundSchleife.
		SndSpieleSoundSchleife(peekw(ptr + 2), peekw(ptr + 4), peekw(ptr + 6));
		return(ptr + 8);
	}
	if (opc == 96) { //StoppeSoundSchleife.
		SndSchleifeAbbruch();
		return(ptr + 2);
	}
	if (opc == 97) { //SoundVolPan.
		SndVolPan(peekw(ptr + 2), peekw(ptr + 4));
		return(ptr + 6);
	}
	if (opc == 67) { //SpieleModule.
		PlSpieleModule(peeks(ptr + 2), peekw(ptr + 6), peekw(ptr + 8));
		return(ptr + 10);
	}
	if (opc == 68) { //StoppeModule.
		PlStoppeModule();
		return(ptr + 2);
	}
	if (opc == 69) { //ModuleWeiter.
		PlModuleWeiter();
		return(ptr + 2);
	}
	if (opc == 65) { //ExternAmigaOS.
		Execute(peeks(ptr + 2), NULL, NULL);
		ScreenToFront(schirm);
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
		*wieder = TRUE;
		AddInventar(peeks(ptr + 8), peeks(ptr + 4), peekw(ptr + 2));
		return(ptr + 12);
	}
	if (opc == 25) { //InventarWeg.
		*wieder = TRUE;
		EntferneInventar(peekw(ptr + 2)); return(ptr + 4);
	}
	if (opc == 20) { //WennGleich.
		*wieder = TRUE;
		if (peekw(ptr + 2) != peekw(ptr + 4)) return(peekl(ptr + 6)); else return(ptr + 10);
	}
	if (opc == 21) { //WennAnders.
		*wieder = TRUE;
		if (peekw(ptr + 2) == peekw(ptr + 4)) return(peekl(ptr + 6)); else return(ptr + 10);
	}
	if (opc == 50) { //WennGrößer.
		*wieder = TRUE;
		if (peekw(ptr + 2) <= peekw(ptr + 4)) return(peekl(ptr + 6)); else return(ptr + 10);
	}
	if (opc == 51) { //WennKleiner.
		*wieder = TRUE;
		if (peekw(ptr + 2) >= peekw(ptr + 4)) return(peekl(ptr + 6)); else return(ptr + 10);
	}
	if (opc == 45) { //WennSichtbar.
		*wieder = TRUE;
		if (SichtWert(peekw(ptr + 2), peekw(ptr + 4))) return(ptr + 10); else return(peekl(ptr + 6));
	}
	if (opc == 46) { //WennUnsichtbar.
		*wieder = TRUE;
		if (SichtWert(peekw(ptr + 2), peekw(ptr + 4))) return(peekl(ptr + 6)); else return(ptr + 10);
	}
	if (opc == 35) { //SetzeVariable.
		*wieder = TRUE;
		SetzeVar(peekw(ptr + 2), peekw(ptr + 4));
		return(ptr + 6);
	}
	if (opc == 36) { //InitVariable.
		*wieder = TRUE;
		VarInit(peekw(ptr + 2), peekw(ptr + 4));
		return(ptr + 6);
	}
	if (opc == 60) { //HolePersVars.
		*wieder = TRUE;
		person = SucheIDPerson(peekw(ptr + 2));
		if (peekw(ptr + 4) > 0) SetzeVar(peekw(ptr + 4), (WORD)person->x);
		if (peekw(ptr + 6) > 0) SetzeVar(peekw(ptr + 6), (WORD)person->y);
		if (peekw(ptr + 8) > 0) SetzeVar(peekw(ptr + 8), person->richtung);
		if (peekw(ptr + 10) > 0) SetzeVar(peekw(ptr + 10), person->aktion);
		return(ptr + 12);
	}
	if (opc == 52) { //AddVariable.
		*wieder = TRUE;
		SetzeVar(peekw(ptr + 2), VarWert(peekw(ptr + 2)) + peekw(ptr + 4));
		return(ptr + 6);
	}
	if (opc == 53) { //SubVariable.
		*wieder = TRUE;
		SetzeVar(peekw(ptr + 2), VarWert(peekw(ptr + 2)) - peekw(ptr + 4));
		return(ptr + 6);
	}
	if (opc == 54) { //MulVariable.
		*wieder = TRUE;
		SetzeVar(peekw(ptr + 2), VarWert(peekw(ptr + 2)) * peekw(ptr + 4));
		return(ptr + 6);
	}
	if (opc == 55) { //DivVariable.
		*wieder = TRUE;
		SetzeVar(peekw(ptr + 2), VarWert(peekw(ptr + 2)) / peekw(ptr + 4));
		return(ptr + 6);
	}
	if (opc == 85) { //Zähle.
		*wieder = TRUE;
		use = VarWert(peekw(ptr + 2)); use ++;
		if (use > peekw(ptr + 6)) use = peekw(ptr + 4);
		SetzeVar(peekw(ptr + 2), use);
		return(ptr + 8);
	}
	if (opc == 91) { //DivRestVariable.
		*wieder = TRUE;
		SetzeVar(peekw(ptr + 2), VarWert(peekw(ptr + 2)) % peekw(ptr + 4));
		return(ptr + 6);
	}
	if (opc == 38) { //Antwort.
		*wieder = TRUE;
		if (peekw(ptr + 8) == peekw(ptr + 10)) AddAntwort(peekw(ptr + 2), peeks(ptr + 4));
		return(ptr + 12);
	}
	if (opc == 78) { //Leistensperre.
		invbar.sperre = TRUE;
		if (invbar.aktiv) InvBarWeg();
		return(ptr + 2);
	}
	if (opc == 79) { //LeistensperreAus.
		invbar.sperre = FALSE;
		return(ptr + 2);
	}
	if (opc == 80) { //Menü.
		zmenu = TRUE;
		return(ptr + 2);
	}
	if (opc == 81) { //SpielEnde.
		FadeOut(4); Ende(); exit(0);
	}
	if (opc == 89) { //HoleZeit.
		if (peekw(ptr + 2) > 0) SetzeVar(peekw(ptr + 2), (WORD)zeit.stunden);
		if (peekw(ptr + 4) > 0) SetzeVar(peekw(ptr + 4), (WORD)zeit.sekunden);
		return(ptr + 6);
	}
	if (opc == 90) { //ZeitDiff.
		use = (peekw(ptr + 2) * 3600) + peekw(ptr + 4);
		use = ((zeit.stunden * 3600) + (WORD)zeit.sekunden) - use;
		SetzeVar(peekw(ptr + 6), (WORD)(use / 3600));
		SetzeVar(peekw(ptr + 8), (WORD)(use % 3600));
		return(ptr + 10);
	}
	if (opc == 100) { //print.
		*wieder = TRUE;
		if (devmodus) printf("%s\n", peeks(ptr + 2));
		return(ptr + 6);
	}
	if (opc == 101) { //printn.
		*wieder = TRUE;
		if (devmodus) printf("%d\n", peekw(ptr + 2));
		return(ptr + 4);
	}

	printf("PC:%ld Opc:%ld\n", ptr, opc);
	Fehler(6, "Unbekannter Skriptbefehl");	
}

UWORD VarWert(UWORD vid) {
	struct VARIABLE *akt;

	akt = rootvar;
	while (akt) {
		if (akt->vid == vid) return(akt->wert);
		akt = akt->next;
	}
	return(0);
}

UWORD VarInit(UWORD vid, UWORD wert) {
	struct VARIABLE *akt;
	struct VARIABLE *neu;
	
	akt = rootvar;
	while (akt) {
		if (akt->vid == vid) return(akt->wert);
		akt = akt->next;
	}
	
	if (neu = malloc(sizeof(struct VARIABLE))) {
		neu->vid = vid;
		neu->wert = wert;

		akt = rootvar;
		rootvar = neu;
		rootvar->next = akt;
	} else Fehler(0, "Variable");
	return(wert);
}

void SetzeVar(UWORD vid, UWORD wert) {
	struct VARIABLE *akt;
	
	akt = rootvar;
	while (akt) {
		if (akt->vid == vid) {
			akt->wert = wert;
			break;
		}
		akt = akt->next;
	}
	if (!akt) VarInit(vid, wert);
}

void EntferneAlleVariablen() {
	struct VARIABLE *akt;
	struct VARIABLE *next;
	
	akt = rootvar;
	while (akt) {
		next = akt->next;
		free(akt);
		akt = next;
	}
	rootvar = NULL;
}

UWORD VarAnzahl() {
	struct VARIABLE *akt;
	UWORD zahl = 0;
	
	akt = rootvar;
	while (akt) {
		zahl++; akt = akt->next;
	}
	return(zahl);
}

BOOL SichtWert(UBYTE ortid, UBYTE elemid) {
	struct SICHT *akt;

	akt = rootsicht;
	while (akt) {
		if ((akt->ortid == ortid) && (akt->elemid == elemid)) return(akt->sichtbar);
		akt = akt->next;
	}
	return(TRUE);
}

BOOL SichtInit(UBYTE ortid, UBYTE elemid, BOOL wert) {
	struct SICHT *akt;
	struct SICHT *neu;
	
	akt = rootsicht;
	while (akt) {
		if ((akt->ortid == ortid) && ( akt->elemid == elemid)) return(akt->sichtbar);
		akt = akt->next;
	}
	
	if (neu = malloc(sizeof(struct SICHT))) {
		neu->ortid = ortid;
		neu->elemid = elemid;
		neu->sichtbar = wert;
		akt = rootsicht;
		rootsicht = neu;
		rootsicht->next = akt;
	} else Fehler(0, "Sichtbarkeitsvariable");
	return(wert);
}

void SetzeSicht(UBYTE ortid, UBYTE elemid, BOOL wert) {
	struct SICHT *akt;
	
	akt=rootsicht;
	while (akt) {
		if ((akt->ortid == ortid) && (akt->elemid == elemid)) {
			akt->sichtbar = wert;
			break;
		}
		akt = akt->next;
	}
	if (!akt) SichtInit(ortid, elemid, wert);
}

void EntferneAlleSichts() {
	struct SICHT *akt;
	struct SICHT *next;
	
	akt = rootsicht;
	while (akt) {
		next = akt->next;
		free(akt);
		akt = next;
	}
	rootsicht = NULL;
}

UWORD SichtAnzahl() {
	struct SICHT *akt;
	UWORD zahl = 0;
	
	akt = rootsicht;
	while (akt) {
		zahl++; akt = akt->next;
	}
	return(zahl);
}
