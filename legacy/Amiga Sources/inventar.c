#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <graphics/gfx.h>
#include <intuition/intuition.h>

#include <proto/graphics.h>
#include <proto/intuition.h>

#include "strukturen.h"
#include "inventar.h"
#include "kernel.h"
#include "grafik.h"
#include "animation.h"

//Systemzeiger
extern struct Window *fenster;

//Maus
extern WORD MausX;
extern WORD MausY;
extern BOOL LMaus;
extern BOOL RMaus;

//Programmsystemvariablen
extern FLOAT frame20;
extern BOOL zmenu;

//Datenstrukturen
extern struct ORT ort;
struct INVBAR invbar = {NULL, NULL, FALSE, FALSE, NULL, 0};
struct INVBEZ inv;

//Elementzeiger
struct INVGEG *rootgeg;

//Aktuelles Objekt
extern char aktbez[61];
extern UWORD aktid;
extern UWORD akttyp;
extern void *aktzeig;
struct INVGEG *aktinv = NULL;

//Aktionen
extern UWORD invbenutzt;



/*==========================================*/
void ErsterGeg() {
	struct INVGEG *akt;
	UWORD z;
	
	akt = rootgeg;
	for(z = 0; z < invbar.pos; z++) akt = akt->next;
	invbar.erster = akt;
}
		

void ZeigeTesteInvBar() {
	struct INVGEG *akt;
	UBYTE z;
	BOOL weiter;

	if (!invbar.ibm->maske) BltBitMapRastPort(invbar.ibm->bild, 0, 0, fenster->RPort, 0, 400, 640, 80, 192);
	else BltMaskBitMapRastPort(invbar.ibm->bild, 0, 0, fenster->RPort, 0, 400, 640, 80, 192, invbar.ibm->maske->Planes[0]);

	akt = invbar.erster; z = 0; weiter = FALSE;
	while (akt && (z<8)) {
		invbar.animp[z] += frame20;
		if (invbar.animp[z] >= akt->ian->ppf + 1) {
			invbar.animnum[z]++;
			if (invbar.animnum[z] >= akt->ian->frames) invbar.animnum[z] = 0;
			invbar.animp[z] = 0;
		}
		BltIANFrame(akt->ian, invbar.animnum[z], 75 + (z * 71), 412, FALSE);
		if ((MausX >= 75 + (z * 71)) && (MausX <= 123 + (z * 71)) && (MausY >= 412) && (MausY <= 460)) {
			strcpy(aktbez, akt->name);
			aktid = akt->id;
			akttyp = 4;
			aktzeig = (void *)akt;
		}
		akt = akt->next; z++;
		if ((z == 8) && akt) weiter = TRUE;
	}
	if ((MausX >= 0) && (MausX <= 56) && (MausY >= 400) && (MausY <= 430)) {
		BltIANFrame(invbar.ian, 0, 0, 400, FALSE);
		if (LMaus) {
			zmenu = TRUE; LMaus = FALSE;
		}
	}
	if ((MausX >= 15) && (MausX <= 56) && (MausY >= 438) && (MausY <= 452) && (invbar.pos > 0)) {
		BltIANFrame(invbar.ian, 1, 0, 400, FALSE);
		if (LMaus) {
			invbar.pos -= 8; ErsterGeg(); LMaus = FALSE;
		}
	}
	if ((MausX >= 15) && (MausX <= 56) && (MausY >= 455) && (MausY <= 470) && weiter) {
		BltIANFrame(invbar.ian, 2, 0, 400, FALSE);
		if (LMaus) {
			invbar.pos += 8; ErsterGeg(); LMaus = FALSE;
		}
	}
}

void InvBarWeg() {
	invbar.aktiv = FALSE;
	RestauriereHintergrund(0, 400, 640, 80);
}

struct INVGEG *AddInventar(STRPTR datei, STRPTR name, UWORD id) {
	struct INVGEG *neu;

	if (neu = malloc(sizeof(INVGEG))) {
		neu->id = id;
		strcpy(neu->name, name);
		strcpy(neu->datei, datei);
		neu->ian = LadeIAN(datei, MASKE_ERSTELLEN);
		neu->next = rootgeg;
		rootgeg = neu;
		invbar.erster = rootgeg;
		invbar.pos = 0;
	} else Fehler(0, datei);
	return(neu)
}

void EntferneInventar(UWORD id) {
	struct INVGEG *akt;
	struct INVGEG *alt;

	if (rootgeg->id == id) {
		alt = rootgeg;
		rootgeg = rootgeg->next;
		EntferneIAN(alt->ian);
		free(alt);
	} else {
		akt = rootgeg->next; alt = rootgeg;
		do {
			if (akt->id == id) {
				alt->next = akt->next;
				EntferneIAN(akt->ian);
				free(akt); break;
			}
			alt = akt; akt = akt->next;
		} while (akt);
	}
	invbar.erster = rootgeg;
	invbar.pos = 0;
}

void EntferneAlleInventar() {
	struct INVGEG *next;
	struct INVGEG *akt;

	akt = rootgeg;
	while (akt) {
		next = akt->next;
		EntferneIAN(akt->ian);
		free(akt);
		akt = next;
	}
	rootgeg = NULL;
	invbar.pos = 0;
	invbar.erster = NULL;
}

UWORD InvAnzahl() {
	struct INVGEG *akt;
	UWORD zahl = 0;
	
	akt = rootgeg;
	while (akt) {
		zahl++; akt = akt->next;
	}
	return(zahl);
}

void BltAktInv(WORD x, WORD y) {
	inv.altx = inv.x;
	inv.alty = inv.y;
	if ((invbenutzt != 0) && aktinv) {

		inv.animp += frame20;
		if (inv.animp >= aktinv->ian->ppf + 1) {
			inv.animnum++;
			if (inv.animnum >= aktinv->ian->frames) inv.animnum = 0;
			inv.animp = 0;
		}

		inv.x = x; inv.y = y;
		BltIANFrame(aktinv->ian, inv.animnum, inv.x, inv.y, FALSE);
	} else inv.y = 0;
}

void BltAktInvWeg() {
	UWORD hohe;
	if (inv.alty >= 0) {
		if (inv.alty <= 432) hohe = 48; else hohe = 480 - inv.alty;
		BltBitMapRastPort(ort.ibm->bild, inv.altx, inv.alty, fenster->RPort, inv.altx, inv.alty, 48, hohe, 192);
	}
}
