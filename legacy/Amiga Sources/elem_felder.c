#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <exec/types.h>

#include "strukturen.h"
#include "elem_felder.h"
#include "vp.h"
#include "kernel.h"

//Maus
extern WORD MausX;
extern WORD MausY;

//Elementzeiger
struct FELD *rootfeld = NULL;

//Datenstrukturen
extern struct ORT ort;

//Aktuelles Objekt
extern char aktbez[61];
extern UWORD aktid;
extern UWORD akttyp;
extern void *aktzeig;

/*=========================FELD Objekte========================*/
struct FELD *AddFeld(UWORD id, WORD lox, WORD loy, WORD rux, WORD ruy, STRPTR name, WORD gehx, WORD gehy) {
	struct FELD *neu;

	if (neu = malloc(sizeof(FELD))) {
		neu->lox = lox; neu->loy = loy;
		neu->rux = rux; neu->ruy = ruy;
		neu->id = id;
		neu->gehx = gehx; neu->gehy = gehy;
		strncpy(neu->name, name, 61);
		neu->sichtbar = TRUE;

		neu->next = rootfeld;
		rootfeld = neu;
	} else Fehler(0, "Feld");
	return(neu);
}

void EntferneFeld(UWORD id) {
	struct FELD *akt;

	SetzeSicht(ort.id, id, FALSE);

	akt = rootfeld;
	while (akt) {
		if (akt->id == id) {
			akt->sichtbar = FALSE;
			break;
		}
		akt = akt->next;
	}
}

void EntferneAlleFelder() {
	struct FELD *next;
	struct FELD *akt;

	akt = rootfeld;
	while (akt) {
		next = akt->next;
		free(akt);
		akt = next;
	}
	rootfeld = NULL;
}

void TesteFelder() {
	struct FELD *akt;

	akt = rootfeld;
	while (akt) {
		if (akt->sichtbar && (akt->id > aktid)) {
			if ((MausX >= akt->lox) && (MausX <= akt->rux) && (MausY >= akt->loy) && (MausY <= akt->ruy)) {
				strcpy(aktbez, akt->name);
				aktid = akt->id;
				akttyp = 1;
				aktzeig = (void *)akt;
			}
		}
		akt = akt->next;
	}
}
