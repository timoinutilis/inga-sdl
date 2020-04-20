#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <graphics/gfx.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <cybergraphx/cybergraphics.h>
#include <exec/exec.h>

#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>
#include <proto/exec.h>

#include "strukturen.h"
#include "grafik.h"
#include "cache.h"
#include "kernel.h"

//Systemzeiger
extern struct Window *fenster;
extern struct Screen *schirm;
extern struct ScreenBuffer *sbuf[2] = {NULL, NULL};
extern struct MsgPort *sbport[2] = {NULL, NULL};
struct Message *sbmsg = NULL;

//Datenstrukturen
extern struct ORT ort;

//Programmsystemvariablen
UBYTE sbnum = 1;
BOOL hell = FALSE;

//Datenstrukturen
struct IAN globian;
UBYTE globrgb[768];
struct RESTAUR {
	BOOL letztes;
	WORD x;
	WORD y;
	UWORD b;
	UWORD h;
	struct RESTAUR *next;
};
struct RESTAUR *rootrest = NULL;

/*==========================Grafik============================*/

struct BitMap *LadeIMP(STRPTR datei) {
	struct CACHE *file;
	struct IMP imp;
	struct BitMap *bitmap = NULL;
	char datimp[100];
	STRPTR p;

	strcpy(datimp, "BitMaps/"); strcat(datimp, datei); strcat(datimp, ".imp");

	MausStatusWarte(TRUE);
	if (!(file = COpen(datimp))) {
		if (p = strrchr(datimp, '_')) {
			strcpy(p, ".imp");
			file = COpen(datimp);
		}
	}
	if (file) {
		CRead(file, &imp, sizeof(struct IMP));
		while (!(bitmap = AllocBitMap(imp.breite, imp.hoehe, 1, NULL, NULL))) {
			if (!CacheAufraumen()) break;
		}
		if (bitmap) CRead(file, bitmap->Planes[0], imp.bytes);
		else Fehler(1, datei);

		CClose(file);
	}
	MausStatusWarte(FALSE);
	return(bitmap);
}

struct IBM *LadeIBM(STRPTR datei, WORD maske) {
	struct CACHE *file;
	char datibm[100];
	struct IBM *ibm = NULL;
	struct BitMap *bitmap = NULL;
	void *rec = NULL;
	struct RastPort rp;

	strcpy(datibm, "BitMaps/"); strcat(datibm, datei); strcat(datibm, ".ibm");

	MausStatusWarte(TRUE);
	if (file = COpen(datibm)) {
		if (ibm = malloc(sizeof(struct IBM))) {
			CRead(file, ibm, sizeof(struct IBM) - 8);
			if (ibm->flags & 1) CRead(file, &globrgb[0], 768);
			if (ibm->flags & 2) {
				CRead(file, &globian, sizeof(struct IAN) - 4);
			} else {
				globian.frames = 1;
				globian.richtung = 0;
				globian.breite = ibm->breite;
				globian.hoehe = ibm->hoehe;
				globian.greifpx = 0;
				globian.greifpy = 0;
				globian.ppf = 0;
			}
			while (!(bitmap = AllocBitMap(ibm->breite, ibm->hoehe, 8, BMF_MINPLANES | BMF_SPECIALFMT, fenster->RPort->BitMap))) {
				if (!CacheAufraumen()) break;
			}
			if (bitmap) {
				rec = CPtr(file, ibm->bpr * ibm->hoehe);
				InitRastPort(&rp);
				rp.BitMap = bitmap;
				WritePixelArray(rec, 0, 0, ibm->bpr, &rp, 0, 0, ibm->breite, ibm->hoehe, RECTFMT_LUT8);
				ibm->bild = bitmap;
				if (maske == MASKE_KEINE) ibm->maske = NULL;
				if (maske == MASKE_DATEI) ibm->maske = LadeIMP(datei);
				if (maske == MASKE_ERSTELLEN) {
					if (ibm->maske = AllocBitMap(ibm->breite, ibm->hoehe, 1, NULL, NULL)) {
						ExtractColor(&rp, ibm->maske, 0, 0, 0, ibm->breite, ibm->hoehe);
						BltBitMap(ibm->maske, 0, 0, ibm->maske, 0, 0, ibm->breite, ibm->hoehe, 80, 0xff, NULL);
					} else Fehler(1, datibm);
				}
			} else Fehler(1, datibm);
		} else Fehler(0, datei);
		CClose(file);
	} else Fehler(2, datibm);
	MausStatusWarte(FALSE);
	return(ibm);
}

void EntferneIBMIMP(struct IBM *ibm) {
	if (ibm) {
		if (ibm->bild) FreeBitMap(ibm->bild);
		if (ibm->maske) FreeBitMap(ibm->maske);
		free(ibm);
	}
}

// Wechselt die Double-Buffer Screens
void BildWechsel() {
	WaitTOF(); 
	while (!ChangeScreenBuffer(schirm, sbuf[sbnum])) WaitTOF();
	WaitPort(sbport[sbnum]);
	while (sbmsg = GetMsg(sbport[sbnum]));
	if (sbnum == 0) sbnum = 1; else sbnum = 0;
	fenster->RPort->BitMap = sbuf[sbnum]->sb_BitMap;
}

void FadeIn(UBYTE tempo) {
	ULONG rgb32[770];
	WORD f, fade;
	
	if (!hell) {
		rgb32[0] = 256L << 16;
		rgb32[769] = 0;
		for(fade = 0; fade < 255; fade += tempo) {
			for(f = 0; f < 256; f++) {
				rgb32[1 + f * 3] = ((ULONG)globrgb[f * 3] * fade) << 16;
				rgb32[2 + f * 3] = ((ULONG)globrgb[f * 3 + 1] * fade) << 16;
				rgb32[3 + f * 3] = ((ULONG)globrgb[f * 3 + 2] * fade) << 16;
			}
			WaitTOF();
			LoadRGB32(&schirm->ViewPort, &rgb32[0]);
		}
		
		for(f = 0; f < 256; f++) {
			rgb32[1 + f * 3] = (ULONG)globrgb[f * 3] << 24;
			rgb32[2 + f * 3] = (ULONG)globrgb[f * 3 + 1] << 24;
			rgb32[3 + f * 3] = (ULONG)globrgb[f * 3 + 2] << 24;
		}
		WaitTOF(); LoadRGB32(&schirm->ViewPort, &rgb32[0]);
		hell = TRUE;
	}
}

void FadeOut(UBYTE tempo) {
	ULONG rgb32[770];
	WORD f, fade;
	
	if (hell) {
		rgb32[0] = 256L << 16;
		rgb32[769] = 0;
		for(fade = 255; fade > 0; fade -= tempo) {
			for(f = 0; f < 256; f++) {
				rgb32[1 + f * 3]=((ULONG)globrgb[f * 3] * fade) << 16;
				rgb32[2 + f * 3]=((ULONG)globrgb[f * 3 + 1] * fade) << 16;
				rgb32[3 + f * 3]=((ULONG)globrgb[f * 3 + 2] * fade) << 16;
			}
			WaitTOF();
			LoadRGB32(&schirm->ViewPort, &rgb32[0]);
		}

		for (f = 0; f < 256; f++) {
			rgb32[1 + f * 3] = 0; rgb32[2 + f * 3] = 0; rgb32[3 + f * 3] = 0;
		}
		WaitTOF(); LoadRGB32(&schirm->ViewPort, &rgb32[0]);
		hell = FALSE;
	}
}

void ZeigeBild(struct IBM *ibm) {
	WORD x, y;
	if (ibm) {
		x = 320 - (ibm->breite >> 1);
		y = 240 - (ibm->hoehe >> 1);
		if ((x != 0) || (y != 0)) {
			SetAPen(fenster->RPort, 0);
			RectFill(fenster->RPort, 0, 0, 640, 480);
		}
		BltBitMapRastPort(ibm->bild, 0, 0, fenster->RPort, x, y, ibm->breite, ibm->hoehe, 192);
		BildWechsel();
		if ((x != 0) || (y != 0)) {
			SetAPen(fenster->RPort, 0);
			RectFill(fenster->RPort, 0, 0, 640, 480);
		}
		BltBitMapRastPort(ibm->bild, 0, 0, fenster->RPort, x, y, ibm->breite, ibm->hoehe, 192);
	} else printf("ZeigeBild NULL!\n");
}

void LadeHintergrund(STRPTR datei) {
	struct CACHE *file;
	char datibm[100];
	void *rec = NULL;
	struct RastPort rp;


	strcpy(datibm, "BitMaps/"); strcat(datibm, datei); strcat(datibm, ".ibm");

	if (!ort.ibm) {
		ort.ibm = malloc(sizeof(struct IBM));
		ort.ibm->bild = AllocBitMap(640, 480, 8, BMF_MINPLANES | BMF_SPECIALFMT, fenster->RPort->BitMap);
	}

	MausStatusWarte(TRUE);
	if (file = COpen(datibm)) {
		CRead(file, ort.ibm, sizeof(struct IBM) - 8);
		if (ort.ibm->flags & 1) CRead(file, &globrgb[0], 768);
		if (ort.ibm->flags & 2) CPtr(file, sizeof(struct IAN) - 4);

		rec = CPtr(file, ort.ibm->bpr * ort.ibm->hoehe);
		InitRastPort(&rp);
		rp.BitMap = ort.ibm->bild;
		WritePixelArray(rec, 0, 0, ort.ibm->bpr, &rp, 0, 0, ort.ibm->breite, ort.ibm->hoehe, RECTFMT_LUT8);
		
		if (ort.ibm->maske) FreeBitMap(ort.ibm->maske);
		ort.ibm->maske = LadeIMP(datei);
		
		CClose(file);

		RestauriereHintergrund(0, 0, 640, 480);

	} else Fehler(2, datibm);
	MausStatusWarte(FALSE);
}

void RestauriereHintergrund(WORD x, WORD y, UWORD breite, UWORD hoehe) {
	struct RESTAUR *neu;
	
	if (neu = malloc(sizeof(RESTAUR))) {
		neu->x = x; neu->y = y;
		neu->b = breite; neu->h = hoehe;
		neu->letztes = FALSE;
		neu->next = rootrest;
		rootrest = neu;
	} else Fehler(0, "Restauration");
}

void Restauration() {
	struct RESTAUR *akt;
	struct RESTAUR *alt;
	
	akt = rootrest; alt = NULL;
	while (akt) {
		BltBitMapRastPort(ort.ibm->bild, akt->x, akt->y, fenster->RPort, akt->x, akt->y, akt->b, akt->h, 192);
		if (akt->letztes) {
			if (alt) alt->next = akt->next; else rootrest = akt->next;
			free(akt);
			if (alt) akt = alt->next; else akt = rootrest;
		} else {
			akt->letztes = TRUE;
			alt = akt; akt = akt->next;
		}
	}
}

void EntferneAlleRestaurationen() {
	struct RESTAUR *next = NULL;
	struct RESTAUR *akt = NULL;

	akt = rootrest;
	while (akt) {
		next = akt->next;
		free(akt);
		akt = next;
	}
	rootrest = NULL;
}
