#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dos/dos.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>

#include <proto/dos.h>
#include <proto/graphics.h>

#include "strukturen.h"
#include "animation.h"
#include "grafik.h"
#include "kernel.h"

//Systemzeiger
extern struct Window *fenster;
extern struct Screen *schirm;

//Programmsystemvariablen

//Datenstrukturen
extern struct IAN globian;
extern struct ORT ort;

/*==========================Animation==========================*/

struct IAN *LadeIAN(STRPTR datei, WORD maske) {
	struct IAN *ian;

	if (ian = malloc(sizeof(IAN))) {
		ian->ibm = LadeIBM(datei, maske);
		memcpy(ian, &globian, sizeof(struct IAN) - 4);
	} else Fehler(0, datei);
	return(ian);
}

void EntferneIAN(struct IAN *ian) {
	if (ian) {
		EntferneIBMIMP(ian->ibm);
		free(ian);
	}
}

void BltIANFrame(struct IAN *ian, UWORD frame, WORD x, WORD y, BOOL maske) {
	WORD desx;
	WORD desy;
	UWORD hohe;

	if (ian) {
		if (ian->richtung == 0) {
			desx = frame * (ian->breite + 1);
			desy = 0;
		} else {
			desx = 0;
			desy = frame * (ian->hoehe + 1);
		}
		x = x - ian->greifpx;
		y = y - ian->greifpy;

		hohe = ian->hoehe;
		if (y + ian->hoehe > 479) {
			hohe = 480 - y;
		} else if (y < 0) {
			hohe = hohe + y;
			desy = desy - y;
			y = 0;
		}
		if (ian->ibm) {
			if (!ian->ibm->maske) {
				BltBitMapRastPort(ian->ibm->bild, desx, desy, fenster->RPort, x, y, ian->breite, hohe, 192);
			} else {
				BltMaskBitMapRastPort(ian->ibm->bild, desx, desy, fenster->RPort, x, y, ian->breite, hohe, 192, ian->ibm->maske->Planes[0]);
			}
		} else printf("BlitIANFrame ibm NULL!\n");
		if (maske && ort.ibm->maske) {
			BltMaskBitMapRastPort(ort.ibm->bild, x, y, fenster->RPort, x, y, ian->breite, ian->hoehe, 192, ort.ibm->maske->Planes[0]);
		}
	} else printf("BlitIANFrame NULL!\n");
}
