#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <exec/types.h>

#include "strukturen.h"
#include "elem_objekte.h"
#include "animation.h"
#include "grafik.h"
#include "vp.h"
#include "kernel.h"

//Maus
extern WORD MausX;
extern WORD MausY;

//Programmsystemvariablen
extern FLOAT frame20;

//Datenstrukturen
extern struct ORT ort;

//Elementzeiger
struct OBJEKT *rootobjekt = NULL;

//Aktuelles Objekt
extern char aktbez[61];
extern UWORD aktid;
extern UWORD akttyp;
extern void *aktzeig;

/*=========================OBJEKT Objekte======================*/
/*-------Datenverwaltung---------*/
struct OBJEKT *AddObjekt(UWORD id, WORD x, WORD y, STRPTR datei, STRPTR name, WORD gehx, WORD gehy) {
	struct OBJEKT *neu;

	if (neu = malloc(sizeof(OBJEKT))) {
		neu->x = x; neu->y = y;
		neu->id = id;
		strncpy(neu->name, name, 61);
		neu->animnum = 0; neu->animp = 0;
		neu->standbild = FALSE;
		neu->ian = LadeIAN(datei, MASKE_KEINE);
		neu->gehx = gehx;
		neu->gehy = gehy;
		neu->sichtbar = TRUE;

		neu->next = rootobjekt;
		rootobjekt = neu;
	} else Fehler(0, datei);
	return(neu)
}

void EntferneObjekt(UWORD id) {
	struct OBJEKT *akt;

	SetzeSicht(ort.id, id, FALSE);

	akt = rootobjekt;
	while (akt) {
		if (akt->id == id) {
			akt->sichtbar = FALSE;
			RestauriereHintergrund(akt->x, akt->y, akt->ian->breite, akt->ian->hoehe);
			break;
		}
		akt = akt->next;
	}
}

void ObjektStandbild(UWORD id, WORD num) {
	struct OBJEKT *akt;

	akt = rootobjekt;
	while (akt) {
		if (akt->id == id) {
			if (num >= 0) {
				akt->standbild = TRUE;
				akt->animnum = num;
			} else {
				akt->standbild = FALSE;
				akt->animnum = 0;
				akt->animp = 0;
			}
			break;
		}
		akt = akt->next;
	}
}

void EntferneAlleObjekte() {
	struct OBJEKT *next;
	struct OBJEKT *akt;

	akt = rootobjekt;
	while (akt) {
		next = akt->next;
		EntferneIAN(akt->ian);
		free(akt);
		akt = next;
	}
	rootobjekt = NULL;
}
/*---------Grafikanzeige----------*/

void BltTesteObjekte() {
	struct OBJEKT *akt;

	akt = rootobjekt;
	while (akt) {
		if (akt->sichtbar) {
			if (!akt->standbild) {
				akt->animp += frame20;
				if (akt->animp >= akt->ian->ppf + 1) {
					akt->animnum++;
					if (akt->animnum >= akt->ian->frames) akt->animnum = 0;
					akt->animp = 0;
				}
			}
			BltIANFrame(akt->ian, akt->animnum, akt->x, akt->y, FALSE);
			if (akt->id > aktid) {
				if ((MausX >= akt->x) && (MausX <= akt->x + akt->ian->breite) && (MausY >= akt->y) && (MausY <= akt->y + akt->ian->hoehe)) {
					strcpy(aktbez, akt->name);
					aktid = akt->id;
					akttyp = 2;
					aktzeig = (void *)akt;
				}
			}
		}
		akt = akt->next;
	}
}
