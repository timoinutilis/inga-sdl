#include <stdlib.h>
#include <stdio.h>

#include <exec/types.h>

#include "strukturen.h"
#include "elem_zierden.h"
#include "animation.h"
#include "grafik.h"
#include "vp.h"
#include "kernel.h"

//Programmsystemvariablen
extern FLOAT frame20;

//Elementzeiger
struct ZIERDE *rootzierde = NULL;

//Datenstrukturen
extern struct ORT ort;

/*=========================ZIERDE Objekte======================*/
/*-------Datenverwaltung---------*/
struct ZIERDE *AddZierde(UWORD id, WORD x, WORD y, STRPTR datei) {
	struct ZIERDE *neu;

	if (neu = malloc(sizeof(ZIERDE))) {
		neu->x = x; neu->y = y;
		neu->id = id;
		neu->animnum = 0; neu->animp = 0;
		neu->ian = LadeIAN(datei, MASKE_KEINE);
		neu->sichtbar = TRUE;

		neu->next = rootzierde;
		rootzierde = neu;
	} else Fehler(0, datei);
	return(neu)
}

void EntferneZierde(UWORD id) {
	struct ZIERDE *akt;

	SetzeSicht(ort.id, id, FALSE);

	akt = rootzierde;
	while (akt) {
		if (akt->id == id) {
			akt->sichtbar = FALSE;
			RestauriereHintergrund(akt->x, akt->y, akt->ian->breite, akt->ian->hoehe);
			break;
		}
		akt = akt->next;
	}
}

void EntferneAlleZierden() {
	struct ZIERDE *next;
	struct ZIERDE *akt;

	akt = rootzierde;
	while (akt) {
		next = akt->next;
		EntferneIAN(akt->ian);
		free(akt);
		akt = next;
	}
	rootzierde = NULL;
}
/*---------Grafikanzeige----------*/

void BltZierden() {
	struct ZIERDE *akt;

	akt = rootzierde;
	while (akt) {
		if (akt->sichtbar) {
			akt->animp += frame20;
			if (akt->animp >= akt->ian->ppf + 1) {
				akt->animnum++;
				if (akt->animnum >= akt->ian->frames) akt->animnum = 0;
				akt->animp = 0;
			}
			BltIANFrame(akt->ian, akt->animnum, akt->x, akt->y, FALSE);
		}
		akt = akt->next;
	}
}
