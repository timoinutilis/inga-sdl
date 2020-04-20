/*=========================================================**
**        Inutilis Grafik Adventure System INGA            **
**---------------------------------------------------------**
**     ©2000-2004 by Inutilis Software / Timo Kloss        **
**=========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <exec/exec.h>
#include <intuition/intuition.h>
#include <intuition/pointerclass.h>
#include <dos/dos.h>
#include <graphics/gfx.h>
#include <diskfont/diskfont.h>
#include <workbench/icon.h>
#include <cybergraphx/cybergraphics.h>
#include <libraries/asl.h>
#include <libraries/lowlevel.h>
#include <devices/timer.h>
#include <workbench/startup.h>
#include <wbstartup.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/diskfont.h>
#include <proto/icon.h>
#include <proto/cybergraphics.h>
#include <proto/asl.h>
#include <proto/lowlevel.h>
#include <proto/timer.h>

#include "strukturen.h"
#include "kernel.h"
#include "grafik.h"
#include "animation.h"
#include "Textausgabe.h"
#include "elem_felder.h"
#include "elem_zierden.h"
#include "elem_objekte.h"
#include "elem_personen.h"
#include "vp.h"
#include "inventar.h"
#include "dialog.h"
#include "ingasound.h"
#include "ingaplayer.h"
#include "ingaaudiocd.h"
#include "menu.h"
#include "cache.h"

//Systemzeiger
struct Library *CyberGfxBase = NULL;
struct Library *LowLevelBase = NULL;
struct Window *fenster = NULL;
struct Screen *schirm = NULL;
struct ScreenBuffer *sbuf[2] = {NULL, NULL};
struct MsgPort *sbport[2] = {NULL, NULL};
BPTR assignlock = NULL;
APTR font = NULL;
BPTR gamedirlock = NULL;

//Maus-/Tastaturvariablen
WORD MausX;
WORD MausY;
BOOL LMaus = FALSE;
BOOL RMaus = FALSE;
BOOL MausEcke = FALSE;
BOOL MausWarte = FALSE;
BOOL MausSichtbar = TRUE;
UBYTE Taste = 0;

//Programmsystemvariablen
char *ver = "$VER: Inga-Engine Version 1.12/11";
char prgname[257] = {0};
extern char cddrive[10];
extern char speicherpfad[300];
struct timeval systime;
FLOAT frame20;
BOOL zmenu = FALSE;
BOOL hauptsichtbar = TRUE;
BOOL erreicht = FALSE;
UWORD iannumstehen = 1;
UWORD iannumgehen = 2;
UBYTE modus = 0; // 0: Nur Skriptbearbeitung.  1: Skriptbearbeitung mit Bild.  2: Bild und Benutzung.
BOOL devmodus = FALSE;
extern ULONG minfree;

//VP-Variablen
BOOL escsprung = FALSE;
ULONG ingaptr = 0;
extern ULONG listeptr;
extern ULONG dialoglisteptr;
extern ULONG escptr;

//Elementzeiger
extern struct FELD *rootfeld;
extern struct OBJEKT *rootobjekt;
extern struct PERSON *rootperson;
extern struct INVGEG *rootgeg;
extern struct ANTWORT *rootantwort;
extern struct INVGEG *aktinv;

//Datenstrukturen
struct ORT ort = {NULL, 0, 0};
extern struct INVBAR invbar;
extern struct INVBEZ inv;
extern struct DIALOG dialog;
extern struct SOUNDBASE soundbase;
struct SPRECH sprech = {{0}, 0, -1, 0, 0, 1.4, FALSE, TRUE, TRUE};
struct SPIELZEIT zeit = {0, 0};
struct IBM *escibm = NULL;
struct NICHTSPROG nichtsprog = {0, 0, 0};

//Aktuelles Objekt
char aktbez[61] = "";
UWORD aktid = 0;
UWORD akttyp = 0;
void *aktzeig = NULL;

//Aktionen
UWORD benutzt = 0;
UWORD invbenutzt = 0;
UWORD angesehen = 0;
UBYTE gesagt = 0;

//Mauszeiger
APTR maus[4] = {NULL, NULL, NULL, NULL};
struct BitMap mausbm[4];
struct BitMap *keinmausbm = NULL;
UWORD ZeigerMaus1[]={
	0xF000,0x0000,0xFE00,0x0000,0xFF80,0x0000,0xFFE0,0x0000, 0x7FF8,0x0000,0x3FFC,0x0000,0x3FFF,0x0000,0x1FFF,0x0000,
	0x1FFF,0x0000,0x0FFF,0x0000,0x0FFE,0x0000,0x0FFE,0x0000, 0x07FF,0x0000,0x07FF,0x8000,0x07FF,0xC000,0x07DF,0xE000,
	0x078F,0xF000,0x070F,0xE000,0x0207,0xF000,0x0003,0xE000, 0x0003,0xC000,0x0001,0x8000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};
UWORD ZeigerMaus2[]={
	0xF000,0x0000,0x8E00,0x0000,0xC180,0x0000,0xE060,0x0000, 0x6018,0x0000,0x3004,0x0000,0x3003,0x0000,0x1801,0x0000,
	0x1803,0x0000,0x0807,0x0000,0x0C06,0x0000,0x0C02,0x0000, 0x0421,0x0000,0x0470,0x8000,0x04F8,0x4000,0x05D8,0x2000,
	0x078C,0x1000,0x070E,0x2000,0x0206,0x7000,0x0003,0xE000, 0x0003,0xC000,0x0001,0x8000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};
UWORD EckeMaus1[]={
	0xF800,0x0000,0xFF80,0x0000,0xFFE0,0x0000,0xFFC0,0x0000, 0x7FE0,0x0000,0x7FC0,0x0000,0x3F80,0x0000,0x3F00,0x0000,
	0x3E00,0x0000,0x3C00,0x0000,0x3800,0x0000,0x1000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};
UWORD EckeMaus2[]={
	0xF800,0x0000,0x8780,0x0000,0xC060,0x0000,0xC040,0x0000, 0x60E0,0x0000,0x61C0,0x0000,0x2380,0x0000,0x2700,0x0000,
	0x2E00,0x0000,0x3C00,0x0000,0x3800,0x0000,0x1000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};
UWORD WarteMaus1[]={
	0x0003,0x8700,0x000F,0xEFC0,0x001F,0xFFE0,0x01DF,0xFFF0, 0x1FFF,0xFFF0,0x3FFF,0xFFF8,0x3FFF,0xFFF8,0x7FFF,0xFFF8,
	0x7FFF,0xFFF8,0x7FFF,0xFFF8,0x7FFF,0xFFF8,0x3FFF,0xFFF0, 0x7FFF,0xFFC0,0x7FFF,0xFFC0,0xFFFF,0xFFC0,0xFFFF,0xFFC0,
	0xFFFF,0xFF80,0xFFFF,0xFF00,0x7FFF,0xFE00,0x7FFF,0xF000, 0x3FFF,0xE000,0x0F9F,0xE000,0x000F,0x8000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};
UWORD WarteMaus2[]={
	0x0003,0x8700,0x000C,0x68C0,0x0010,0x1020,0x01D0,0x0010, 0x1E30,0x0F10,0x2010,0x0208,0x2000,0x0408,0x400F,0xCF88,
	0x4000,0x8018,0x6001,0x00F8,0x71E2,0x0078,0x2044,0x0070, 0x408F,0xC040,0x41E0,0x00C0,0x8000,0x01C0,0x8000,0x13C0,
	0xC000,0x1F80,0xC040,0x1F00,0x78E0,0x3E00,0x7FF0,0x7000, 0x3FFF,0xE000,0x0F9F,0xE000,0x000F,0x8000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};


/*=========================Diverses==========================*/

void StoppeReden() {
	if (sprech.pers >= 0) {
		if (SucheIDPerson(sprech.pers)->aktion == AKT_REDEN) PersonenAktion(sprech.pers, AKT_NICHTS, 0, 0, 0, 0);
		sprech.pers = -1; sprech.text[0] = 0; sprech.zeitz = 0;
		if (sprech.sound) {
			if (soundbase.speech) {
				SndSageAbbruch();
				// Warte, bis die Sprachausgabe abgebrochen wurde...
				while (soundbase.speech) Delay(2);
			}
			sprech.sound = FALSE;
		}
	}
}

/*===========================System============================*/

void MausTastaturStatus() {
	struct IntuiMessage *mes;

	while (mes = (struct IntuiMessage *)GetMsg(fenster->UserPort)) {
		switch (mes->Class) {
			case IDCMP_MOUSEMOVE:
			MausX = mes->MouseX; // X-Position der Maus
			MausY = mes->MouseY; // Y-Position der Maus
			break;

			case IDCMP_MOUSEBUTTONS:
			// Wenn Maustaste gedrückt wird...
			if (mes->Code == 104) LMaus = TRUE;
			if (mes->Code == 105) RMaus = TRUE;
			// ACHTUNG: Wert nicht ständig auf TRUE setzen, wenn die Maustaste geHALTEN wird!
			break;

			case IDCMP_RAWKEY:
			// Raw-Code einer gedrückten Taste.
			Taste = mes->Code;
			break;

		}
		ReplyMsg((struct Message *)mes);
	}
}

void ErschaffeMause() {
	BYTE z;

	for (z = 0; z < 3; z++) {
		InitBitMap(&mausbm[z], 2, 32, 32);
	}
	mausbm[0].Planes[0] = (PLANEPTR)ZeigerMaus1;
	mausbm[0].Planes[1] = (PLANEPTR)ZeigerMaus2;
	mausbm[1].Planes[0] = (PLANEPTR)EckeMaus1;
	mausbm[1].Planes[1] = (PLANEPTR)EckeMaus2;
	mausbm[2].Planes[0] = (PLANEPTR)WarteMaus1;
	mausbm[2].Planes[1] = (PLANEPTR)WarteMaus2;
	for (z = 0; z < 3; z++) {
		maus[z] = NewObject(NULL, "pointerclass",
			POINTERA_BitMap, &mausbm[z],
			POINTERA_WordWidth, 2,
			POINTERA_XResolution, POINTERXRESN_HIRES,
			POINTERA_YResolution, POINTERYRESN_HIGHASPECT,
			TAG_DONE);
	}
	SetWindowPointer(fenster, WA_Pointer, maus[0], TAG_DONE);

	if (keinmausbm = AllocBitMap(16, 1, 2, BMF_CLEAR, NULL)) {
		maus[3] = NewObject(NULL, "pointerclass",
			POINTERA_BitMap, keinmausbm,
			TAG_DONE);
	} else Fehler(1, "Mauspfeil");
}

void EntferneMause() {
	BYTE z;

	ClearPointer(fenster);
	for (z = 0; z < 4; z++) {
		if (maus[z] != NULL) DisposeObject(maus[z]);
	}
	FreeBitMap(keinmausbm);
}

void Maus() {
	UBYTE n;

	if (MausSichtbar) {
		n = MAUS_ZEIGER;
		if (MausEcke) n = MAUS_ECKE;
		if (MausWarte) n = MAUS_WARTE;
	} else n = MAUS_UNSICHTBAR;
	if (maus[n]) SetWindowPointer(fenster, WA_Pointer, maus[n], TAG_DONE);
}

void StandartMauszeiger() {
	if (maus[MAUS_ZEIGER]) SetWindowPointer(fenster, WA_Pointer, maus[MAUS_ZEIGER], TAG_DONE);
}

void MausStatusEcke(BOOL w) {
	MausEcke = w;
	Maus();
}

void MausStatusWarte(BOOL w) {
	MausWarte = w;
	Maus();
}

void MausStatusSichtbar(BOOL w) {
	MausSichtbar = w;
	Maus();
}

void EntwicklerAktion() {
	char aktion[6];
	char eingabe[3][100];
	UWORD n;

	WBenchToFront();
	printf("1. Sprung ausführen\n");
	printf("2. Gegenstand aufnehmen\n");
	printf("3. Variable prüfen/ändern\n");
	printf("q  Spiel beenden\n");
	printf("->"); gets(aktion);
	switch (aktion[0]) {
		case '1':
			printf("Adresse: "); gets(eingabe[0]);
			if (eingabe[0][0] > 0) {
				ingaptr = atol(eingabe[0]);
				modus = 1;
			}
		break;

		case '2':
			printf("Datei: "); gets(eingabe[0]);
			printf("Name:  "); gets(eingabe[1]);
			printf("ID:    "); gets(eingabe[2]);
			AddInventar(eingabe[0], eingabe[1], atol(eingabe[2]));
		break;

		case '3':
			printf("Nummer: "); gets(eingabe[0]);
			if (eingabe[0][0] > 0) {
				n = atol(eingabe[0]);
				printf("Wert (%ld): ", VarWert(n)); gets(eingabe[0]);
				if (eingabe[0][0] > 0) SetzeVar(n, atol(eingabe[0]));
			}
		break;

		case 'q':
		Ende();
		exit(0);
		break;
	}
	printf("\n");
	ScreenToFront(schirm);
}

/*==========================Starter/Beender====================*/

ULONG ScreenMode() {
	ULONG disid;
	char varstr[31];
	LONG l;
	struct ScreenModeRequester *req;
	ULONG keys;
	BOOL wahl = FALSE;

	disid = BestCModeIDTags(CYBRBIDTG_Depth, 8, CYBRBIDTG_NominalWidth, 640, CYBRBIDTG_NominalHeight, 480, TAG_DONE);
	keys = GetKey();
	l = GetVar("IngaScreenMode", varstr, 30, GVF_GLOBAL_ONLY);

	if ((disid == INVALID_ID) && (l == -1)) {
		Meldung("Die automatische ScreenMode-Wahl ist fehlgeschlagen.\nWähle im gleich erscheinenden Requester einen Modus aus.\nDieser wird in der Environment-Variable 'IngaScreenMode'\nabgespeichert, kann aber neu gewählt werden, wenn man\nbeim Starten dieses Spiels die linke Amiga-Taste gedrückt\nhält.");
		wahl = TRUE;
	}
	if (keys & LLKF_LAMIGA) {
		Meldung("Der gleich zu wählende ScreenMode wird in der\nEnvironment-Variable 'IngaScreenMode' gespeichert\nund dann bei jedem Start benutzt.\nWenn man den Requester abbricht, so wird die\nVariable gelöscht.");
		wahl = TRUE;
	}

	if (wahl) {
		if (req = AllocAslRequest(ASL_ScreenModeRequest, NULL)) {
			if (AslRequestTags(req, ASLSM_TitleText, "640x480 - 8 Bit", TAG_DONE)) {
				disid = req->sm_DisplayID;
				sprintf(varstr, "0x%lx", disid);
				SetVar("IngaScreenMode", varstr, -1, GVF_GLOBAL_ONLY | GVF_SAVE_VAR);
			} else {
				if (DeleteVar("IngaScreenMode", GVF_GLOBAL_ONLY)) {
					DeleteFile("ENVARC:IngaScreenMode");
					Meldung("Die Environment-Variable für den\nScreenMode wurde gelöscht");
				}
			}
			FreeAslRequest(req);
		} else Fehler(4, "ASL-Requester");
	} else {
		if (l >= 0) disid=strtoul(varstr, NULL, 0);
	}

	return(disid);
}

void Start() {
	struct TextAttr textattr = {"Inga.font", 15, 0, 0};
	ULONG disid;
	ULONG rgb32[770];
	UWORD f;
	DiskObject *info;
	BOOL aktivsound = FALSE;
	BOOL aktivplayer = FALSE;
	BOOL aktivaudiocd = FALSE;
	STRPTR str;
	BPTR startdirlock = NULL;

	if (!(CyberGfxBase = OpenLibrary("cybergraphics.library", 0)))
		Fehler(3, "cybergraphics");

	if (!(LowLevelBase = OpenLibrary("lowlevel.library", 0)))
		Fehler(3, "lowlevel");

	disid = ScreenMode();
	if (disid == INVALID_ID) Fehler(5, "");

	// komplett schwarze Farbpalette zum Screen-Öffnen
	rgb32[0] = 256L << 16; rgb32[769] = 0;
	for (f = 0; f < 256; f++) {
		rgb32[1 + f * 3] = 0; rgb32[2 + f * 3] = 0; rgb32[3 + f * 3] = 0;
	}
	if (!(schirm = OpenScreenTags(NULL,
		SA_Width, 640,
		SA_Height, 480,
		SA_Depth, 8,
		SA_DisplayID, disid,
		SA_Type, CUSTOMSCREEN,
		SA_Draggable, FALSE,
		SA_Colors32, rgb32,
		TAG_DONE))) Fehler(4, "Bildschirm");

	if (!(fenster = OpenWindowTags(NULL,
		WA_Width, 640,
		WA_Height, 480,
		WA_Borderless, TRUE,
		WA_CustomScreen, schirm,
		WA_Flags, WFLG_REPORTMOUSE | WFLG_RMBTRAP | WFLG_ACTIVATE,
		WA_IDCMP, IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY,
		TAG_DONE))) Fehler(4, "Fenster");

	sbuf[0] = AllocScreenBuffer(schirm, NULL, SB_SCREEN_BITMAP);
	sbuf[1] = AllocScreenBuffer(schirm, NULL, SB_COPY_BITMAP);
	if (sbuf[0] && sbuf[1]) {
		fenster->RPort->BitMap = sbuf[1]->sb_BitMap;
		sbport[0] = CreateMsgPort(); sbuf[0]->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = sbport[0];
		sbport[1] = CreateMsgPort(); sbuf[1]->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = sbport[1];
		if (!sbport[0] && !sbport[1]) Fehler(4, "Bildpuffer-Ports");
	} else Fehler(4, "Bildpuffer");

	if (assignlock = Lock("Fonts/", ACCESS_READ)) {
		AssignAdd("FONTS", assignlock);
	}

	if (font = OpenDiskFont(&textattr)) {
		SetFont(fenster->RPort, font);
	} else Fehler(4, "Inga.font");

	// Merkmale des Piktogramms...
	if (info = GetDiskObject(prgname)) {
		if (FindToolType((STRPTR *)info->do_ToolTypes, "DEVMODE")) devmodus = TRUE;
		if (FindToolType((STRPTR *)info->do_ToolTypes, "SOUND")) aktivsound = TRUE;
		if (FindToolType((STRPTR *)info->do_ToolTypes, "PLAYER")) aktivplayer = TRUE;
		if (FindToolType((STRPTR *)info->do_ToolTypes, "AUDIOCD")) aktivaudiocd = TRUE;
		if (str = FindToolType((STRPTR *)info->do_ToolTypes, "CDDRIVE")) strcpy(cddrive, str);
		if (str = FindToolType((STRPTR *)info->do_ToolTypes, "GAMEPATH")) {
			if (gamedirlock = Lock(str, ACCESS_READ)) {
				startdirlock = CurrentDir(gamedirlock);
				NameFromLock(startdirlock, speicherpfad, 300);
				UnLock(startdirlock);
			} else Fehler(4, "Spielpfad");
		}
		if (str = FindToolType((STRPTR *)info->do_ToolTypes, "MINFREE")) minfree = atol(str)*1048576;
		FreeDiskObject(info);
	}

	ErschaffeMause();

	// Werden ggf. vom Sound-Plug-In auf TRUE gesetzt...
	soundbase.soundplugin = FALSE;
	soundbase.speech = FALSE;

	MausStatusSichtbar(FALSE);
	LadeMenu(); Prozess(2, 9); BildWechsel();
	LadeINGA(); Prozess(3, 9); BildWechsel();
	if (aktivaudiocd) StarteAudioCDTreiber();
	Prozess(4, 9); BildWechsel();
	if (aktivsound) StarteIngaSound();
	Prozess(5, 9); BildWechsel();
	if (aktivplayer) LadePlayer();
	Prozess(6, 9); BildWechsel();
	invbar.ibm = LadeIBM("Inventarleiste", MASKE_DATEI); Prozess(7, 9); BildWechsel();
	invbar.ian = LadeIAN("InventarleisteKnoepfe", MASKE_DATEI); Prozess(8, 9); BildWechsel();
	escibm = LadeIBM("Esc", MASKE_ERSTELLEN);
	AddPerson(0, 320, 360, "Hauptperson", ""); Prozess(9, 9); BildWechsel();
	FadeOut(4);
	MausStatusSichtbar(TRUE);
}

void Ende() {
	EntferneAllePersonen(TRUE);
	EntferneAlleZierden();
	EntferneAlleObjekte();
	EntferneAlleFelder();
	EntferneAlleVariablen();
	EntferneAlleSichts();
	EntferneAlleInventar();
	EntferneAlleAntworten();
	EntferneAlleRestaurationen();
	EntferneAudioCDTreiber();
	EntferneIngaSound();
	EntfernePlayer();
	EntferneIBMIMP(ort.ibm);
	EntferneIBMIMP(invbar.ibm);
	EntferneIAN(invbar.ian);
	EntferneIBMIMP(escibm);
	EntferneINGA();
	EntferneMenu();
	EntleereCache();
	EntferneMause();
	if (assignlock) RemAssignList("FONTS", assignlock);
	if (sbport[0]) DeleteMsgPort(sbport[0]);
	if (sbport[1]) DeleteMsgPort(sbport[1]);
	if (fenster && sbuf[0]) fenster->RPort->BitMap = sbuf[0]->sb_BitMap;
	if (sbuf[0]) FreeScreenBuffer(schirm, sbuf[0]);
	if (sbuf[1]) FreeScreenBuffer(schirm, sbuf[1]);
	if (fenster) CloseWindow(fenster);
	if (schirm) CloseScreen(schirm);
	if (font) CloseFont(font);
	if (gamedirlock) UnLock(gamedirlock);
	CloseLibrary(CyberGfxBase);
	CloseLibrary(LowLevelBase);
}

void Fehler(UWORD num, STRPTR t) {
	struct EasyStruct estr = {
		sizeof(struct EasyStruct),
		0,
		"Inga-Adventure",
		"FEHLER: %s: %s",
		"Spiel beenden"
	};
	STRPTR fehl[] = {
		"Zu wenig Hauptspeicher",
		"Zu wenig Grafikspeicher",
		"Konnte Datei nicht öffnen",
		"Konnte Library nicht öffnen",
		"Konnte nicht öffnen",
		"Konnte keinen passenden CGX-Bildschirmmodus finden",
		"Fehler im Skript",
	};

	Ende();
	EasyRequest(NULL, &estr, NULL, fehl[num], t);

	exit(0);
}

void Meldung(STRPTR t) {
	struct EasyStruct estr = {
		sizeof(struct EasyStruct),
		0,
		"Inga-Adventure",
		t,
		"Weiter"
	};
	EasyRequest(NULL, &estr, NULL);
}

/*=============================Hauptprogramm===================================*/

void hauptteil() {
	struct FELD *feld = NULL;
	struct OBJEKT *objekt = NULL;
	struct PERSON *person = NULL;
	struct PERSON *hauptperson = NULL;
	struct timeval timev;
	struct timeval alttime;
	BOOL wieder;
	FLOAT dx, dy, dist;

	Start();

	do {
		GetSysTime(&systime);

		/*====Framemessung====*/
		timev.tv_secs = systime.tv_secs; timev.tv_micro = systime.tv_micro;
		SubTime(&timev, &alttime); alttime.tv_secs = systime.tv_secs; alttime.tv_micro = systime.tv_micro;
		timev.tv_micro = timev.tv_micro / 1000;
		frame20 = (FLOAT)timev.tv_micro / 50;
		// frame20 soll 1 sein, wenn 20 fps gezeigt werden.
		// Bei z.B. 40 fps soll es 0,5 sein, und bei 10 fps gleich 2, usw.

		zeit.sekunden += frame20 / 20;
		if (zeit.sekunden >= 3600) {
			zeit.sekunden -= 3600; zeit.stunden++;
		}

		/*====Skript bearbeitung====*/
		if (modus < 2) {
			if (escsprung) {
				ingaptr = escptr; escptr = 0; escsprung = FALSE;
			}
			do {
				ingaptr = LaufeINGA(ingaptr, &wieder);
			} while (wieder);
			nichtsprog.frame = 0;
		}

		/*====Bild====*/
		if (modus > 0) {
			MausTastaturStatus();
			BltBitMapRastPort(ort.ibm->bild, 1, 1, fenster->RPort, 1, 1, escibm->breite, escibm->hoehe, 192);
			BltBezeichnungWeg();
			BltAktInvWeg();
			BltPersonenWeg();
			Restauration();
			aktbez[0] = 0;; aktid = 0;
			BltZierden();
			BltTesteObjekte();
			TesteFelder();
			SortierePersonen();
			BltTestePersonen();
			if (invbar.aktiv) {
				aktbez[0] = 0; aktid = 0;
				ZeigeTesteInvBar();
			}
			BltAktInv(MausX, MausY);
			if ((modus != 2) || (dialog.aktiv)) aktbez[0] = 0;
			Bezeichne(MausX, MausY, aktbez);
			if (dialog.aktiv) ZeigeTesteDialog();
			MeldeAusgabe();
			if (escptr > 0) BltMaskBitMapRastPort(escibm->bild, 0, 0, fenster->RPort, 1, 1, escibm->breite, escibm->hoehe, 192, escibm->maske->Planes[0]);
			TesteAudioCD();

			if (sprech.pers >= 0) {
				sprech.zeitz -= frame20;
				if (RMaus) {
					StoppeReden();
					if (modus != 2) RMaus = FALSE;
				}
				// ((kein Sound-Plug-In) && (Sprechzeit zuende)) || ((Sound-Plug-In) && (z.Z. keine Sprachausgabe))
				if ((!sprech.sound && (sprech.zeitz <= 0)) || (sprech.sound && !soundbase.speech)) {
					StoppeReden();
				} else {
					// ((Sprachtext inaktiv) && (z.Z. Sprachausgabe))
					if (!sprech.untertitel && sprech.sound) sprech.text[0] = 0;
				}
			}
			Spreche(sprech.x, sprech.y, sprech.text);

			BildWechsel();

			//Tastatur Abfrage
			// <ESC>
			if ((Taste == 69) && (escptr > 0)) {
				escsprung = TRUE; Melde("Abbruch", 0); StoppeReden();
			}
			// <F1>
			if ((Taste == 80) && (sprech.tempo > 0.5)) sprech.tempo -= 0.2;
			// <F2>
			if ((Taste == 81) && (sprech.tempo < 2.5)) sprech.tempo += 0.2;
			// <F1> oder <F2>
			if ((Taste == 80) || (Taste == 81)) Melde("Sprechdauer: %ld", (UWORD)(sprech.tempo * 5));
			if (soundbase.soundplugin) {
				// <F3>
				if (Taste == 82) {
					sprech.untertitel = !sprech.untertitel;
					if (sprech.untertitel) Melde("Untertitel ein", 0); else Melde("Untertitel aus", 0);
				}
				// <F4>
				if (Taste == 83) {
					sprech.spracheaktiv = !sprech.spracheaktiv;
					if (sprech.spracheaktiv) Melde("Sprachausgabe ein", 0); else Melde("Sprachausgabe aus", 0);
				}
			}
			// <F5>
			if ((Taste == 84) && (modus == 2)) zmenu = TRUE;
			// <F10>
			if ((Taste == 89) && devmodus) EntwicklerAktion();
			Taste = 0;

			// Menübildschirm zeigen?
			if (zmenu) {
				zmenu = FALSE; invbar.aktiv = FALSE;
				StoppeReden();
				Menubildschirm();
			}

		}

		/*====Benutzung====*/
		if (modus == 2) {
			if ((MausY > 474) && !dialog.aktiv && !invbar.sperre) invbar.aktiv = TRUE;
			if (((MausY < 400) || dialog.aktiv) && invbar.aktiv) InvBarWeg();

			if (LMaus) {
				if (!invbar.aktiv && !dialog.aktiv) {
					aktinv = NULL; MausStatusEcke(FALSE);
					if (aktid == 0) {
						if (hauptsichtbar) PersonenAktion(0, AKT_LAUFEN, MausX, MausY, iannumgehen, 0);
						benutzt = 0; invbenutzt = 0; angesehen = 0;
					} else {
						StoppeReden();
						if (akttyp == 1) {
							feld = (FELD *)aktzeig;
							if (hauptsichtbar) PersonenAktion(0, AKT_LAUFEN, feld->gehx, feld->gehy, iannumgehen, 0); else erreicht = TRUE;
							benutzt = aktid;
						}
						if (akttyp == 2) {
							objekt=(OBJEKT *)aktzeig;
							if (hauptsichtbar) PersonenAktion(0, AKT_LAUFEN, objekt->gehx, objekt->gehy, iannumgehen, 0); else erreicht = TRUE;
							benutzt = aktid;
						}
						if (akttyp == 3) {
							hauptperson = SucheIDPerson(0);
							person = (PERSON *)aktzeig;
							person->isfaktiv = FALSE;
							person->aktion = AKT_NICHTS;
							if (hauptsichtbar) {
								dx = hauptperson->x-person->x;
								dy = hauptperson->y-person->y;
								if (((dx * dx) < 1) && ((dy * dy) < 1)) {
									if (hauptperson->richtung == RI_RECHTS) dx = 1; else dx = -1;
								}
								dist = sqrt((dx * dx) + (dy * dy));
								dx = dx / dist * ((person->ian->breite + hauptperson->ian->breite) >> 1); dy = dy / dist * 40;
								if (invbenutzt == 0) {
									PersonenAktion(0, AKT_LAUFEN, (WORD)(person->x + dx), (WORD)(person->y + dy), iannumgehen, 2);
								} else {
									PersonenAktion(0, AKT_LAUFEN, (WORD)(person->x + dx), (WORD)(person->y + dy), iannumgehen, 0);
								}
							} else erreicht = TRUE;
							benutzt = aktid;
						}
					}
				} else {
					if (invbar.aktiv) {
						if ((aktid != 0) && (akttyp == 4)) {
							if (invbenutzt == 0) {
								MausStatusEcke(TRUE);
								invbenutzt = aktid;
								aktinv = (INVGEG *)aktzeig;
								inv.animnum = 0; inv.animp = 0;
							} else {
								MausStatusEcke(FALSE);
								InvBarWeg();
								benutzt = aktid; aktinv = NULL;
								MausStatusSichtbar(FALSE);
								ingaptr = listeptr; modus = 0;
							}
						}
					}
					if (dialog.aktiv) {
						if ((aktid != 0) && (akttyp == 5)) {
							DialogWeg(); gesagt = aktid;
							MausStatusSichtbar(FALSE);
							ingaptr = dialoglisteptr; modus = 0;
						}
					}
				}
			}

			if (RMaus) {
				if (!dialog.aktiv) {
					if (aktid != 0) {
						StoppeReden();
						angesehen = aktid; benutzt = 0;
						if (akttyp == 1) {
							feld = (FELD *)aktzeig;
							if (hauptsichtbar) PersonenAktion(0, AKT_LAUFEN, feld->gehx, feld->gehy, iannumgehen, 2); else erreicht = TRUE;
						}
						if (akttyp == 2) {
							objekt = (OBJEKT *)aktzeig;
							if (hauptsichtbar) PersonenAktion(0, AKT_LAUFEN, objekt->gehx, objekt->gehy, iannumgehen, 2); else erreicht = TRUE;
						}
						if (akttyp == 3) {
							person = (PERSON *)aktzeig;
							PersonRichtung(0, (WORD)person->x, (WORD)person->y);
							MausStatusSichtbar(FALSE);
							ingaptr = listeptr; modus = 0;
						}
						if (akttyp == 4) {
							hauptperson = SucheIDPerson(0);
							hauptperson->richtung = RI_VORNE;
							MausStatusSichtbar(FALSE);
							ingaptr = listeptr; modus = 0;
						}
					} else {
						PersonRichtung(0, MausX, MausY);
					}
				}
			}

			if (erreicht) {
				MausStatusSichtbar(FALSE);
				ingaptr = listeptr; modus = 0;
				erreicht = FALSE;
			}

			if (nichtsprog.dauer > 0) {
				if (invbenutzt > 0) nichtsprog.frame = 0;
				nichtsprog.frame += frame20;
				if ((UWORD)nichtsprog.frame >= nichtsprog.dauer) {
					MausStatusSichtbar(FALSE);
					ingaptr = nichtsprog.ptr; modus = 1;
				}
			}
		}

		LMaus = FALSE; RMaus = FALSE;

	} while (TRUE);
}

/*=============================CLI/WB Start================================*/

void main() {
	GetProgramName(prgname, 256);
	hauptteil();
}

void wbmain(struct WBStartup *argmsg) {
	if (argmsg->sm_ArgList->wa_Lock) CurrentDir(argmsg->sm_ArgList->wa_Lock);
	strcpy(prgname, argmsg->sm_ArgList->wa_Name);
	hauptteil();
}
