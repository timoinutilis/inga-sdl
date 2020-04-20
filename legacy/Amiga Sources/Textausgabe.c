#include <string.h>
#include <stdio.h>

#include <graphics/gfx.h>
#include <intuition/intuition.h>

#include <proto/graphics.h>

#include "strukturen.h"
#include "Textausgabe.h"

//Systemzeiger
extern struct Window *fenster;
extern struct Screen *schirm;

//Programmsystemvariablen
UBYTE sysfar[6] = {1, 0, 4, 5, 19, 0};
char meld[80];
UWORD meldl, meldz;

//Datenstrukturen
extern struct ORT ort;
struct BEZ bez = {0, -1, 0, 0, 0, 0};
struct BEZ spr = {0, -1, 0, 0, 0, 0};


/*==========================Textausgabe========================*/

void Schreibe(WORD x, WORD y, WORD far, STRPTR str) {
	WORD lang;

	lang = strlen(str);
	y = y + fenster->RPort->TxBaseline;
	SetABPenDrMd(fenster->RPort, sysfar[0], 0, 0);
	Move(fenster->RPort, x + 2, y + 1); Text(fenster->RPort, str, lang);
	Move(fenster->RPort, x, y + 1); Text(fenster->RPort, str, lang);
	Move(fenster->RPort, x + 1, y + 2); Text(fenster->RPort, str, lang);
	Move(fenster->RPort, x + 1, y); Text(fenster->RPort, str, lang);
	SetAPen(fenster->RPort, far);
	Move(fenster->RPort, x + 1, y + 1); Text(fenster->RPort, str, lang);
}

void SchreibeOR(WORD x, WORD y, WORD far, STRPTR str) {
	SetABPenDrMd(fenster->RPort, far, 0, 0);
	Move(fenster->RPort, x, y + fenster->RPort->TxBaseline); Text(fenster->RPort, str, strlen(str));
}

void Bezeichne(WORD x, WORD y, STRPTR str) {
	bez.altx = bez.x;
	bez.alty = bez.y;
	bez.altlen = bez.len;
	if (strlen(str) > 0) {
		bez.len = TextLength(fenster->RPort, str, strlen(str)) + 3;
		bez.x = x - (bez.len >> 1);
		bez.y = y - fenster->RPort->TxHeight - 2;
		if (bez.x < 0) bez.x = 0;
		if (bez.x + bez.len > 639) bez.x = 639 - bez.len;
		if (bez.y < 0) bez.y = 0;
		Schreibe(bez.x, bez.y, sysfar[2], str);
	} else bez.len = 0;
}

void Spreche(WORD x, WORD y, STRPTR str) {
	spr.altx = spr.x;
	spr.alty = spr.y;
	spr.altlen = spr.len;
	if (str[0]) {
		spr.len = TextLength(fenster->RPort, str, strlen(str)) + 3;
		spr.x = x - (spr.len >> 1);
		spr.y = y - fenster->RPort->TxHeight - 2;
		if (spr.x < 0) spr.x = 0;
		if (spr.x + spr.len > 639) spr.x = 639 - spr.len;
		if (spr.y < 0) spr.y = 0;
		Schreibe(spr.x, spr.y, sysfar[1], str);
	} else spr.len = 0;
}

void Melde(STRPTR meldung, UWORD wert) {
	UWORD len;
	
	sprintf(meld, meldung, wert);
	meldz = 20;
	len = TextLength(fenster->RPort, meld, strlen(meld)) + 3;
	if (len > meldl) meldl = len;
}

void MeldeAusgabe() {
	if (meldz > 0) {
		meldz--; if (meldz == 0) meldl = 0;
		if (meldz > 2) Schreibe(2, 2, sysfar[3], meld);
	}
}

void MeldungAbbruch() {
	meldz = 0; meldl = 0;
}

void BltBezeichnungWeg() {
	if ((bez.alty >= 0) && (bez.altlen > 0)) {
		BltBitMapRastPort(ort.ibm->bild, bez.altx, bez.alty, fenster->RPort, bez.altx, bez.alty, bez.altlen, fenster->RPort->TxHeight + 3, 192);
	}
	if ((spr.alty >= 0) && (spr.altlen > 0)) {
		BltBitMapRastPort(ort.ibm->bild, spr.altx, spr.alty, fenster->RPort, spr.altx, spr.alty, spr.altlen, fenster->RPort->TxHeight + 3, 192);
	}
	if (meldz > 0) {
		BltBitMapRastPort(ort.ibm->bild, 0, 0, fenster->RPort, 0, 0, meldl, fenster->RPort->TxHeight + 4, 192);
	}
}
