#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dos/dos.h>
#include <exec/exec.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include "strukturen.h"
#include "menu.h"
#include "grafik.h"
#include "animation.h"
#include "textausgabe.h"
#include "elem_felder.h"
#include "elem_zierden.h"
#include "elem_objekte.h"
#include "elem_personen.h"
#include "vp.h"
#include "inventar.h"
#include "ingasound.h"
#include "ingaaudiocd.h"
#include "kernel.h"

//Systemzeiger
extern struct Screen *schirm;
extern struct Window *fenster;
extern struct ScreenBuffer *sbuf[2];

//Maus
extern WORD MausX;
extern WORD MausY;
extern BOOL LMaus;

//Programmsystemvariablen
extern char *ver;
extern UBYTE sysfar[6];
extern UBYTE modus;
extern BOOL devmodus;
char speicherpfad[300] = "";
extern char hauptipe[31];

//VP-Variablen
extern ULONG ingaptr;
extern ULONG escptr;

//Elementzeiger
extern VARIABLE *rootvar;
extern SICHT *rootsicht;
extern INVGEG *rootgeg;

//Datenstrukturen
extern ORT ort;
extern SPRECH sprech;
extern UBYTE globrgb[768];
extern SPIELZEIT zeit;
UBYTE menurgb[768];
struct MENU menu = {NULL, NULL, NULL, NULL};

//Menu intern
char punktname[10][60];
BYTE aktpunkt;
UWORD frame;
UWORD pausez;
char slotname[6][60];
BPTR savelock = NULL;
BPTR spiellock = NULL;


/*======================================================*/
void Prozess(UWORD num, UWORD von) {
	UWORD x, y, br, ho;

	br = menu.prozessleer->breite;
	ho = menu.prozessleer->hoehe;
	x = 320 - (br >> 1);
	y = 240 - (ho >> 1);
	if (num > von) num = von;

	if (!menu.prozessleer->maske)
		BltBitMapRastPort(menu.prozessleer->bild, 0, 0, fenster->RPort, x, y, br, ho, 192);
	else BltMaskBitMapRastPort(menu.prozessleer->bild, 0, 0, fenster->RPort, x, y, br, ho, 192, menu.prozessleer->maske->Planes[0]);

	if (num > 0) {
		if (!menu.prozessvoll->maske)
			BltBitMapRastPort(menu.prozessvoll->bild, 0, 0, fenster->RPort, x, y, br * num / von, ho, 192);
		else BltMaskBitMapRastPort(menu.prozessvoll->bild, 0, 0, fenster->RPort, x, y, br * num / von, ho, 192, menu.prozessvoll->maske->Planes[0]);
	}
}

void SpeicherDir() {
	if (speicherpfad[0] && !savelock) {
		if (savelock = Lock(speicherpfad, ACCESS_READ)) {
			spiellock = CurrentDir(savelock);
		}
	}
}

void SpielDir() {
	if (spiellock) {
		CurrentDir(spiellock); spiellock = NULL;
		UnLock(savelock); savelock = NULL;
	}
}

void LadeSlotnamen() {
	BPTR file;
	UBYTE z;

	SpeicherDir();
	if (file = Open("Saves/Index", MODE_OLDFILE)) {
		Read(file, &slotname[0][0], 6 * 60);
		Close(file);
	} else {
		if (file = Lock("Saves/", ACCESS_READ)) {
			UnLock(file);
		} else {
			if (file = CreateDir("Saves")) UnLock(file);
		}

		for(z = 0; z < 6; z++) {
			strcpy(slotname[z], "---");
		}
	}
	SpielDir();
}

void SpeichereSlotnamen() {
	BPTR file;
	
	SpeicherDir();
	if (file = Open("Saves/Index", MODE_NEWFILE)) {
		Write(file, &slotname[0][0], 6 * 60);
		Close(file);
	}
	SpielDir();
}

void Registriert(STRPTR text) {
	BPTR file;
	ULONG ps;
	ULONG *psw;
	UWORD z;
	UBYTE reg[100];
	BOOL regis = FALSE;
	
	if (file = Open("Dats/reg", MODE_OLDFILE)) {
		Read(file, reg, 100);
		Close(file);
		
		ps = 0; z = 0;
		while (reg[z] != 0) {
			reg[z] = reg[z] + 23;
			ps = ps + reg[z];
			z++;
		}
		psw = (ULONG *)&reg[96];
		if (*psw == ps) {
			z = 0;
			do {
				if (reg[z] == '|') {
					reg[z] = 0;
					break;
				}
			} while (reg[z++] != 0);
			strcpy(text, "Willkommen zu '");
			strcat(text, reg);
			strcat(text, "'!");
			regis = TRUE;
		}
	}
	if (!regis) {
		strcpy(text, "Dieses Spiel darf nicht veröffentlicht werden, da es noch nicht bei Inutilis registriert ist!");
		ps = 0; z = 0;
		while (text[z] != 0) {
			ps = ps + text[z];
			z++;
		}
		if (ps != 8905) Ende();
	}
}

void LadeMenu() {
	char text[150];

	menu.bild = LadeIBM("Menue", MASKE_KEINE);
	CopyMem(globrgb, menurgb, 768);
	ZeigeBild(menu.bild);
	Registriert(text);
	SchreibeOR(10, 10, 255, text); SchreibeOR(10, 460, 255, &ver[6]); BildWechsel();
	SchreibeOR(10, 10, 255, text); SchreibeOR(10, 460, 255, &ver[6]); FadeIn(4);
	menu.prozessleer = LadeIBM("ProzessLeer", MASKE_DATEI);
	Prozess(0, 9); BildWechsel();
	menu.prozessvoll = LadeIBM("ProzessVoll", MASKE_DATEI);
	Prozess(1, 9); BildWechsel();
	menu.punkt = LadeIAN("Menuepunkt", MASKE_ERSTELLEN);
	LadeSlotnamen();
}

void EntferneMenu() {
	EntferneIBMIMP(menu.prozessvoll);
	EntferneIBMIMP(menu.prozessleer);
	EntferneIAN(menu.punkt);
	EntferneIBMIMP(menu.bild);
}

void TesteBltPunkte() {
	WORD z = 1;
	UWORD mittet, mitteb;

	mittet = 15 >> 1;
	mitteb = menu.punkt->hoehe >> 1;
	aktpunkt = -1;
	
	BltBitMapRastPort(menu.bild->bild, 220, 100, fenster->RPort, 220, 100, menu.punkt->breite, 380, 192);
	Schreibe(320 - (TextLength(fenster->RPort, punktname[0], strlen(punktname[0])) >> 1), 100 - mittet, 255, punktname[0]);
	SchreibeOR(10, 460, 255, &ver[6]);

	do {
		if ((MausY > 80 + (z * 40)) && (MausY < 120 + (z * 40))) {
			BltIANFrame(menu.punkt, frame, 220, 100 - mitteb + (z * 40), FALSE);
			Schreibe(260, 100 - mittet + (z * 40), 255, punktname[z]);
			aktpunkt = z;
		} else {
			BltIANFrame(menu.punkt, 0, 220, 100 - mitteb + (z * 40), FALSE);
			Schreibe(260, 100 - mittet + (z * 40), 254, punktname[z]);
		}
		z++;
	} while (punktname[z][0]);

	if (pausez >= menu.punkt->ppf) {
		frame++;
		if (frame >= menu.punkt->frames) frame = 1;
		pausez = 0;
	}
	pausez++;
}

void Eingabe(UBYTE slot) {
	struct IntuiMessage *mes;
	UWORD y;
	UWORD mittet, mitteb;
	char name[62];
	BOOL ende = FALSE;
	UWORD z;
	UWORD len;
	UBYTE blink = 0;
	
	mittet = 15 >> 1;
	mitteb = menu.punkt->hoehe >> 1;
	
	y = 100 - mittet + (slot * 40);
	if (strcmp(slotname[slot - 1], "---") == 0) {
		strcpy(name, "Neu");
	} else {
		strcpy(name, slotname[slot - 1]);
	}
	len = strlen(name);
	
	ModifyIDCMP(fenster, IDCMP_MOUSEBUTTONS | IDCMP_VANILLAKEY);

	do {
		BltBitMapRastPort(menu.bild->bild, 220, 80 + (slot * 40), fenster->RPort, 220, 80 + (slot * 40), 420, 40, 192);
		BltIANFrame(menu.punkt, frame, 220, 100 - mitteb + (slot * 40), FALSE);
		Schreibe(260, 100 - mittet + (slot * 40), 255, name);
		if (blink < 5) Schreibe(260 + TextLength(fenster->RPort, name, len), 100 - mittet + (slot * 40), 255, "_");
		BildWechsel();

		if (pausez >= menu.punkt->ppf) {
			frame++;
			if (frame >= menu.punkt->frames) frame = 1;
			pausez = 0;
		}
		pausez++;
		
		blink++; if (blink > 10) blink = 0;

		while (mes = (struct IntuiMessage *)GetMsg(fenster->UserPort)) {
			if (mes->Class == IDCMP_VANILLAKEY) {
				if (mes->Code == 13) ende = TRUE;
				if (mes->Code == 8) {
					if (len > 0) {
						name[len - 1] = 0; len--;
					}
				}
				if (mes->Code > 30) {
					//Zeichen anhängen.
					if (TextLength(fenster->RPort, name, len) < 360) {
						for (z = 0; z < 60; z++) {
							if (name[z] == 0) {
								name[z] = mes->Code;
								name[z + 1] = 0; len++;
								break;
							}
						}
					}
				}
			}
			if (mes->Class == IDCMP_MOUSEBUTTONS) {
				if (mes->Code == 104) ende = TRUE;
			}
			ReplyMsg((struct Message *)mes);
		}
	} while (!ende);
	strcpy(slotname[slot-1], name);
	ModifyIDCMP(fenster, IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY);
}

BOOL SpielstandSpeichern(UBYTE slot) {
	BOOL ok = FALSE;
	BPTR file;
	char dat[30];
	struct SAVE save;
	struct PERSON *person;
	APTR mem;
	ULONG ptr;
	struct VARIABLE *aktvar;
	struct SICHT *aktsicht;
	struct INVGEG *aktgeg;
	UWORD insg;
	FLOAT z = 0;
	WORD altz = -1;
	FLOAT schritt;
	
	SpeicherDir();
	sprintf(dat, "Saves/Stand%ld.sav", slot);
	if (file = Open(dat, MODE_NEWFILE)) {
		MausStatusWarte(TRUE);
		insg = VarAnzahl() + SichtAnzahl() + InvAnzahl() + 1;
		schritt = 21 / (FLOAT)insg;
		TesteBltPunkte(); Prozess(0, 20); BildWechsel();

		//Grunddaten sichern.
		person = SucheIDPerson(0);
		save.ortptr = ort.ptr;
		save.x = (WORD)person->x;
		save.y = (WORD)person->y;
		save.sprechtempo = sprech.tempo;
		save.untertitel = sprech.untertitel;
		save.spracheaktiv = sprech.spracheaktiv;
		save.zeit.sekunden = zeit.sekunden;
		save.zeit.stunden = zeit.stunden;
		strcpy(save.hauptipe, hauptipe);
		save.anzvar = VarAnzahl();
		save.anzsicht = SichtAnzahl();
		save.anzgeg = InvAnzahl();
		Write(file, &save, sizeof(SAVE));
		z = z + schritt;
		if ((WORD)z > altz) {
			altz = (WORD)z; Prozess((UWORD)z, 20); BildWechsel();
		}

		//Variablen sichern.
		if (mem = malloc(save.anzvar * sizeof(VARIABLE))) {
			aktvar = rootvar; ptr = (ULONG)mem;
			while (aktvar) {
				CopyMem(aktvar, (APTR)ptr, sizeof(VARIABLE));
				ptr = ptr + sizeof(VARIABLE);
				aktvar = aktvar->next;
				z = z + schritt;
				if ((WORD)z > altz) {
					altz = (WORD)z; Prozess((UWORD)z, 20); BildWechsel();
				}
			}
			Write(file, mem, save.anzvar * sizeof(VARIABLE));
			free(mem);
		
			//Sichtbarkeit sichern.
			if (mem = malloc(save.anzsicht * sizeof(SICHT))) {
				aktsicht = rootsicht; ptr = (ULONG)mem;
				while (aktsicht) {
					CopyMem(aktsicht, (APTR)ptr, sizeof(SICHT));
					ptr = ptr + sizeof(SICHT);
					aktsicht = aktsicht->next;
					z = z + schritt;
					if ((WORD)z > altz) {
						altz = (WORD)z; Prozess((WORD)z, 20); BildWechsel();
					}
				}
				Write(file, mem, save.anzsicht * sizeof(SICHT));
				free(mem);
		
				//Inventar sichern.
				if (mem = malloc(save.anzgeg * sizeof(INVGEG))) {
					aktgeg = rootgeg; ptr=(ULONG)mem;
					while (aktgeg) {
						CopyMem(aktgeg, (APTR)ptr, sizeof(INVGEG));
						ptr = ptr + sizeof(INVGEG);
						aktgeg = aktgeg->next;
						z = z + schritt;
						if ((WORD)z > altz) {
							altz = (WORD)z; Prozess((UWORD)z, 20); BildWechsel();
						}
					}
					Write(file, mem, save.anzgeg * sizeof(INVGEG));
					free(mem); ok = TRUE;
				}
			}
		}
		
		Close(file);
		MausStatusWarte(FALSE);
	}
	SpielDir();
	return(ok);
}

BOOL SpielstandLaden(UBYTE slot) {
	BOOL ok = FALSE;
	BPTR file;
	char dat[30];
	struct SAVE save;
	APTR mem;
	struct PERSON *person;
	struct VARIABLE *variable;
	struct SICHT *sicht;
	struct INVGEG *geg;
	UWORD insg;
	FLOAT z = 0;
	WORD altz = -1;
	FLOAT schritt;
	UWORD i;
	char eingabe[20];
	
	SpeicherDir();
	sprintf(dat, "Saves/Stand%ld.sav", slot);
	if (file = Open(dat, MODE_OLDFILE)) {
		MausStatusWarte(TRUE);
		SpielDir();
	
		//Grunddaten laden.
		Prozess(0, insg); BildWechsel();
		Read(file, &save, sizeof(SAVE));
		person = SucheIDPerson(0);
		person->x = save.x;
		person->y = save.y;
		ingaptr = save.ortptr;
		sprech.tempo = save.sprechtempo;
		sprech.untertitel = save.untertitel;
		sprech.spracheaktiv = save.spracheaktiv;
		zeit.sekunden = save.zeit.sekunden;
		zeit.stunden = save.zeit.stunden;
		ErsetzeHauptpersonIPE(save.hauptipe);
		insg = save.anzvar + save.anzsicht + save.anzgeg + 1;
		schritt = 21 / (FLOAT)insg;
		z = z + schritt;
		if (altz < (WORD)z) {
			altz = (WORD)z; Prozess((UWORD)z, 20); BildWechsel();
		}
		if (devmodus) {
			WBenchToFront();
			printf("Zeiger (%ld): ", ingaptr);
			gets(eingabe); if (eingabe[0] > 0) ingaptr = atol(eingabe);
			ScreenToFront(schirm);
		}
		
		//Variablen laden.
		if (mem = malloc(save.anzvar * sizeof(VARIABLE))) {
			variable = (struct VARIABLE *)mem;
			Read(file, mem, save.anzvar * sizeof(VARIABLE));
			for (i = 0; i < save.anzvar; i++) {
				SetzeVar(variable->vid, variable->wert);
				variable++;
				z = z + schritt;
				if (altz < (WORD)z) {
					altz = (WORD)z; Prozess((UWORD)z, 20); BildWechsel();
				}
			}
			free(mem);
		
			//Sichts laden.
			if (mem = malloc(save.anzsicht * sizeof(SICHT))) {
				sicht = (struct SICHT *)mem;
				Read(file, mem, save.anzsicht * sizeof(SICHT));
				for (i = 0; i < save.anzsicht; i++) {
					SetzeSicht(sicht->ortid, sicht->elemid, sicht->sichtbar);
					sicht++;
					z = z + schritt;
					if (altz < (WORD)z) {
						altz = (WORD)z; Prozess((UWORD)z, 20); BildWechsel();
					}
				}
				free(mem);
		
				//Inventar laden.
				if (mem = malloc(save.anzgeg * sizeof(INVGEG))) {
					geg = (struct INVGEG *)mem;
					Read(file, mem, save.anzgeg * sizeof(INVGEG));
					for (i = 0; i < save.anzgeg; i++) {
						AddInventar(geg->datei, geg->name, geg->id);
						geg++;
						z = z + schritt;
						if (altz < (WORD)z) {
							altz = (WORD)z; Prozess((UWORD)z, 20); BildWechsel();
						}
					}
					free(mem); ok = TRUE;
				}
			}
		}
		Close(file);
		MausStatusWarte(FALSE);
	}
	SpielDir();
	return(ok);
}

void AndereSeite(UBYTE num) {
	UBYTE z;

	ZeigeBild(menu.bild);
	switch (num) {
		case 0:
			strcpy(punktname[0], "");
			strcpy(punktname[1], "Spiel neu beginnen");
			strcpy(punktname[2], "Spielstand laden");
			strcpy(punktname[3], "Spielstand speichern");
			strcpy(punktname[4], "Weiterspielen");
			strcpy(punktname[5], "Spiel beenden");
			punktname[6][0] = 0;
		break;
		case 1:
			strcpy(punktname[0], "Willst du das Spiel wirklich beenden?");
			strcpy(punktname[1], "Ja");
			strcpy(punktname[2], "Nein");
			punktname[3][0] = 0;
		break;
		case 2:
			strcpy(punktname[0], "Willst du das Spiel wirklich neu beginnen?");
			strcpy(punktname[1], "Ja");
			strcpy(punktname[2], "Nein");
			punktname[3][0] = 0;
		break;
		case 3:
			strcpy(punktname[0], "Wähle Spielstand zum Laden!");
			for(z = 0; z < 6; z++) {
				strcpy(punktname[z + 1], slotname[z]);
			}
			strcpy(punktname[7], "Zurück zum Hauptmenü");
			punktname[8][0] = 0;
		break;
		case 4:
			strcpy(punktname[0], "Wähle Spielstand zum Speichern!");
			for(z = 0; z < 6; z++) {
				strcpy(punktname[z + 1], slotname[z]);
			}
			strcpy(punktname[7], "Zurück zum Hauptmenü");
			punktname[8][0] = 0;
		break;
		case 5:
			strcpy(punktname[0], "Spielstand konnte nicht geladen werden!");
			strcpy(punktname[1], "Zurück");
			punktname[2][0] = 0;
		break;
		case 6:
			strcpy(punktname[0], "Spielstand konnte nicht gespeichert werden!");
			strcpy(punktname[1], "Zurück");
			punktname[2][0] = 0;
		break;
	}
}

void Menubildschirm() {
	struct PERSON *person;
	UBYTE puffrgb[768];
	BOOL raus = FALSE;
	UBYTE seite;
	UBYTE farpuf;

	FadeOut(6);
	CopyMem(globrgb, puffrgb, 768);
	CopyMem(menurgb, globrgb, 768);
	StandartMauszeiger();
	ZeigeBild(menu.bild); FadeIn(6);

	farpuf = sysfar[0];
	sysfar[0] = FindColor(schirm->ViewPort.ColorMap, 0, 0, 0, 255);
	frame = 1; pausez = 0;
	seite = 0; AndereSeite(0);
	
	do {
		MausTastaturStatus();
		TesteBltPunkte();
		BildWechsel();
		TesteAudioCD();

		if (LMaus) {
			LMaus = FALSE;
			if (seite == 0) {
				switch (aktpunkt) {
					case 1: seite = 2; AndereSeite(2); break;
					case 2: seite = 3; AndereSeite(3); break;
					case 3: seite = 4; AndereSeite(4); break;
					case 4:
						//Weiter spielen.
						FadeOut(6);
						CopyMem(puffrgb, globrgb, 768);
						BltBitMap(ort.ibm->bild, 0, 0, sbuf[0]->sb_BitMap, 0, 0, ort.ibm->breite, ort.ibm->hoehe, 192, 0xFF, NULL);
						BltBitMap(ort.ibm->bild, 0, 0, sbuf[1]->sb_BitMap, 0, 0, ort.ibm->breite, ort.ibm->hoehe, 192, 0xFF, NULL);
						BltZierden(); BltTesteObjekte(); SortierePersonen(); BltTestePersonen(); BildWechsel();
						Maus();
						FadeIn(6);
						raus = TRUE;
					break;
					case 5: seite = 1; AndereSeite(1); break;
				}
			} else if (seite == 1) {
				switch (aktpunkt) {
					case 1: FadeOut(4); Ende(); exit(0); break;
					case 2: seite = 0; AndereSeite(0); break;
				}
			} else if (seite == 2) {
				switch (aktpunkt) {
					case 1:
						//Neu starten.
						modus = 0; ingaptr = 0; escptr = 0; ort.ptr = 0;
						zeit.stunden = 0; zeit.sekunden = 0;
						EntferneAllePersonen(FALSE);
						EntferneAlleZierden();
						EntferneAlleObjekte();
						EntferneAlleFelder();
						EntferneAlleVariablen();
						EntferneAlleSichts();
						EntferneAlleInventar();
						person=SucheIDPerson(0);
						person->x = 320;
						person->y = 360;
						raus = TRUE;
						FadeOut(6);
						SndSchleifeAbbruch(); StoppeCD();
					break;
					case 2: seite = 0; AndereSeite(0); break;
				}
			} else if (seite == 3) {
				//Laden Menü.
				if ((aktpunkt > 0) && (aktpunkt < 7)) {
					if (strcmp(slotname[aktpunkt - 1], "---") != 0) {
						modus = 0;
						EntferneAllePersonen(FALSE);
						EntferneAlleZierden();
						EntferneAlleObjekte();
						EntferneAlleFelder();
						EntferneAlleVariablen();
						EntferneAlleSichts();
						EntferneAlleInventar();
						SndSchleifeAbbruch(); StoppeCD();
						if (!SpielstandLaden(aktpunkt)) {
							ingaptr = 0; escptr = 0; ort.ptr = 0;
							zeit.stunden = 0; zeit.sekunden = 0;
							person = SucheIDPerson(0);
							person->x = 320;
							person->y = 360;
							seite = 5; AndereSeite(5);
						} else raus = TRUE;
					}
				}
				if (aktpunkt == 7) {
					seite = 0; AndereSeite(0);
				}
			} else if (seite == 4) {
				//Speichern Menü.
				if ((aktpunkt > 0) && (aktpunkt < 7)) {
					Eingabe(aktpunkt);
					if (SpielstandSpeichern(aktpunkt)) {
						SpeichereSlotnamen();
						seite = 0; AndereSeite(0);
					} else {
						strcpy(slotname[aktpunkt], "---");
						seite = 6; AndereSeite(6);
					}
				}
				if (aktpunkt == 7) {
					seite = 0; AndereSeite(0);
				}
			} else if (seite == 5) {
				if (aktpunkt == 1) {
					seite = 3; AndereSeite(3);
				}
			} else if (seite == 6) {
				if (aktpunkt == 1) {
					seite = 4; AndereSeite(4);
				}
			}
		}
	} while (!raus);
	sysfar[0] = farpuf;
}
