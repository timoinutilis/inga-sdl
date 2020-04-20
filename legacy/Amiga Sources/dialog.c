#include <stdlib.h>
#include <string.h>

#include <graphics/gfx.h>
#include <intuition/intuition.h>

#include <proto/graphics.h>

#include "strukturen.h"
#include "dialog.h"
#include "Textausgabe.h"
#include "grafik.h"
#include "kernel.h"

//Systemzeiger
extern struct Window *fenster;

//Maus
extern WORD MausX;
extern WORD MausY;

//Programmsystemvariablen
extern UBYTE sysfar[6];

//Elementzeiger
struct ANTWORT *rootantwort = NULL;

//Aktuelles Objekt
extern UWORD aktid;
extern UWORD akttyp;

//Datenstrukturen
struct DIALOG dialog = {0, FALSE};


/*===========================================================*/
struct ANTWORT *AddAntwort(UBYTE id, STRPTR text) {
	struct ANTWORT *neu;

	if (neu = malloc(sizeof(ANTWORT))) {
		neu->aid = id;
		strcpy(neu->text, text);
		neu->next = rootantwort;
		rootantwort = neu;
		dialog.anz++;
	} else Fehler(0, "Antwort");
	return(neu);
}

void EntferneAlleAntworten() {
	struct ANTWORT *akt;
	struct ANTWORT *next;
	
	akt = rootantwort;
	while (akt) {
		next = akt->next;
		free(akt);
		akt = next;
	}
	rootantwort = NULL;
	dialog.anz=0;
}

void ZeigeTesteDialog() {
	UWORD y;
	UWORD len;
	UBYTE far;
	struct ANTWORT *akt;
	
	y = 470 - (dialog.anz * (fenster->RPort->TxHeight + 3));
	akt = rootantwort;
	while (akt) {
		len = TextLength(fenster->RPort, akt->text, strlen(akt->text)) + 3;
		if ((MausY >= y) && (MausY <= y + fenster->RPort->TxHeight) && (MausX >= 320 - (len >> 1)) && (MausX <= 320 + (len >> 1))) {
			far = sysfar[5]; akttyp = 5; aktid = akt->aid;
		} else far = sysfar[4];
		Schreibe(320 - (len >> 1), y, far, akt->text);
		y = y + fenster->RPort->TxHeight + 3;
		akt = akt->next;
	}
}

void DialogWeg() {
	struct ANTWORT *akt;
	UWORD y, len;
	UWORD maxlen = 0;

	dialog.aktiv = FALSE;

	y = 470 - (dialog.anz * (fenster->RPort->TxHeight + 3));
	akt = rootantwort;
	while (akt) {
		len = TextLength(fenster->RPort, akt->text, strlen(akt->text)) + 3;
		if (len > maxlen) maxlen = len;
		akt = akt->next;
	}
	RestauriereHintergrund(320 - (maxlen >> 1), y, maxlen, 480 - y);
	EntferneAlleAntworten();
}
