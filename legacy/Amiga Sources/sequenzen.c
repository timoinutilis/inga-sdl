#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <exec/exec.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <cybergraphx/cybergraphics.h>
#include <devices/timer.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/timer.h>

#include "strukturen.h"
#include "grafik.h"
#include "Textausgabe.h"
#include "cache.h"
#include "ingasound.h"
#include "ingaaudiocd.h"
#include "kernel.h"

//Systemzeiger
extern struct Window *fenster;

//Programmsystemvariablen
extern struct timeval systime;

//Maus / Tastatur
extern BOOL LMaus;
extern UBYTE Taste;

/*====================================================*/
void Sequenz(STRPTR datei) {
	BPTR file;
	APTR mem;
	ULONG usel;
	UWORD i;
	LONG len;
	struct IBM *ibm = NULL;
	struct BitMap *bitmap = NULL;
	char dat[200];
	UBYTE *z;
	STRPTR zeile[256];
	UBYTE zanz = 0;
	UBYTE art = 0, bund = 0, farbe = 255, fadein = 2, fadeout = 2;
	UWORD thoehe = 3, ttiefe = 476, txheight;
	WORD zscrolly, tlen;
	FLOAT scrolly, abstand;
	struct timeval alttime;
	struct timeval timev;
	BOOL abbruch = FALSE;

	strcpy(dat, "Dats/"); strcat(dat, datei); strcat(dat,".isq");

	if (file = Open(dat, MODE_OLDFILE)) {
		Seek(file, 0, OFFSET_END); len = Seek(file, 0, OFFSET_BEGINNING);
		if (mem = malloc(len)) {
			Read(file, mem, len);
			for(z = mem; (ULONG)z < (ULONG)mem + len; z++) {
				if (*z == 0x0A) *z = 0;
			}
			
			while(!(bitmap = AllocBitMap(640, 480, 8, BMF_MINPLANES | BMF_SPECIALFMT, fenster->RPort->BitMap))) {
				if (!CacheAufraumen()) break;
			}
			
			txheight = fenster->RPort->TxHeight;
			z = mem;
			MausStatusSichtbar(FALSE);
			do {
				if (z[0] == '$') {
					if (zanz > 0) {
						BildWechsel();
						if (bitmap) BltBitMapRastPort(bitmap, 0, 0, fenster->RPort, 0, 0, 640, 480, 192);
						if (art == 0) {
							scrolly = (FLOAT)(ttiefe - txheight);
							GetSysTime(&alttime); abstand = 0;
							do {
								if (bitmap) BltBitMapRastPort(bitmap, 0, thoehe + txheight, fenster->RPort, 0, thoehe + txheight, 640, (WORD)abstand << 1, 192);
								for (i = 0; i < zanz; i++) {
									zscrolly = (UWORD)scrolly + (i * (txheight + 3));
									if ((zscrolly < (ttiefe - txheight)) && (zscrolly > (thoehe - 2))) {
										tlen = TextLength(fenster->RPort, zeile[i], strlen(zeile[i])) + 3;
										if (bitmap && (zscrolly + (WORD)abstand + txheight + 4 + ((WORD)abstand << 1) < 480)) {
											switch (bund) {
												case 0: BltBitMapRastPort(bitmap, 10, zscrolly, fenster->RPort, 10, zscrolly, tlen, txheight + 4 + ((WORD)abstand << 1), 192); break;
												case 1: BltBitMapRastPort(bitmap, 320 - (tlen >> 1), zscrolly, fenster->RPort, 320 - (tlen >> 1), zscrolly, tlen, txheight + 4 + ((WORD)abstand << 1), 192); break;
												case 2: BltBitMapRastPort(bitmap, 630 - tlen, zscrolly, fenster->RPort, 630 - tlen, zscrolly, tlen, txheight + 4 + ((WORD)abstand << 1), 192); break;
											}
										}
										if (zscrolly > thoehe) {
											switch (bund) {
												case 0: SchreibeOR(10, zscrolly, farbe, zeile[i]); break;
												case 1: SchreibeOR(320 - (tlen >> 1), zscrolly, farbe, zeile[i]); break;
												case 2: SchreibeOR(630 - tlen, zscrolly, farbe, zeile[i]); break;
											}
										}
									}
								}
								if (bitmap) {
									BltBitMapRastPort(bitmap, 0, thoehe, fenster->RPort, 0, thoehe, 640, txheight, 192);
									BltBitMapRastPort(bitmap, 0, ttiefe - txheight, fenster->RPort, 0, ttiefe - txheight, 640, txheight, 192);
								}
								BildWechsel();
								
								GetSysTime(&systime);
								timev.tv_secs = systime.tv_secs; timev.tv_micro = systime.tv_micro;
								SubTime(&timev, &alttime);
								alttime.tv_secs = systime.tv_secs; alttime.tv_micro = systime.tv_micro;
								timev.tv_micro = timev.tv_micro >> 5;
								abstand = (FLOAT)timev.tv_micro / 2200;
								scrolly = scrolly - abstand;

								TesteAudioCD();

								MausTastaturStatus();
								if (Taste == 69) abbruch = TRUE; // Escape gedrückt
							} while ((zscrolly > thoehe) && !abbruch);
						}
						if (art > 0) {
							if (art == 1) zscrolly = 10;
							if (art == 2) zscrolly = 240 - ((zanz * (fenster->RPort->TxHeight + 3)) >> 1);
							if (art == 3) zscrolly = 470 - (zanz * (fenster->RPort->TxHeight + 3));
							for (i = 0; i < zanz; i++) {
								tlen = TextLength(fenster->RPort, zeile[i], strlen(zeile[i])) + 3;
								switch (bund) {
									case 0: SchreibeOR(10, zscrolly, farbe, zeile[i]); break;
									case 1: SchreibeOR(320 - (tlen >> 1), zscrolly, farbe, zeile[i]); break;
									case 2: SchreibeOR(630 - tlen, zscrolly, farbe, zeile[i]); break;
								}						
								zscrolly = zscrolly + fenster->RPort->TxHeight + 3;
							}
							BildWechsel();
						}
					}
					if (!abbruch) {
						zanz = 0;
						if (z[1] == ':') {
							FadeOut(fadeout);
							ibm = LadeIBM(&z[2], MASKE_KEINE);
							ZeigeBild(ibm); EntferneIBMIMP(ibm);
							FadeIn(fadein);
							if (bitmap) BltBitMap(fenster->RPort->BitMap, 0, 0, bitmap, 0, 0, 640, 480, 192, 0xFF, NULL);
						}
						if (z[1] == '!') SndSage(&z[2], 320);
						if (z[1] == 'W') {
							usel = atol(&z[2]);
							GetSysTime(&alttime);
							alttime.tv_secs += usel;
							do {
								MausTastaturStatus();
								if (Taste == 69) abbruch = TRUE; // Escape gedrückt
								GetSysTime(&systime);
								TesteAudioCD();
							} while (!abbruch && (CmpTime(&alttime, &systime) == -1));
						}
						if (z[1] == 'M') {
							LMaus = FALSE;
							do {
								MausTastaturStatus();
								if (Taste == 69) abbruch = TRUE; // Escape gedrückt
								GetSysTime(&systime);
								TesteAudioCD();
							} while (!LMaus && !abbruch);
							LMaus = FALSE;
						}
						if (z[1] == 'A') art = z[2] - 48;
						if (z[1] == 'B') bund = z[2] - 48;
						if (z[1] == 'O') {
							usel = atol(&z[2]); thoehe = usel;
							if (thoehe < 3) thoehe = 3;
						}
						if (z[1] == 'U') {
							usel = atol(&z[2]); ttiefe = usel;
							if (ttiefe > 476) ttiefe = 476;
						}
						if (z[1] == 'F') {usel = atol(&z[2]); farbe = usel}
						if (z[1] == '<') {usel = atol(&z[2]); fadein = usel}
						if (z[1] == '>') {usel = atol(&z[2]); fadeout = usel}
					}

				} else {
					zeile[zanz++] = z;
				}

				z = (UBYTE *)((ULONG)z + strlen(z) + 1);
			} while (((ULONG)z < len + (ULONG)mem) && !abbruch);
			SndSageAbbruch();
			FadeOut(fadeout);
			free(mem);
			if (bitmap) FreeBitMap(bitmap);
		} else Fehler(0, dat);
		Close(file);
	} else Fehler(2, dat);
}
