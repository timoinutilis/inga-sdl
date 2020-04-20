#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <dos/dos.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>

#include <proto/dos.h>
#include <proto/graphics.h>

#include "strukturen.h"
#include "elem_personen.h"
#include "elem_objekte.h"
#include "animation.h"
#include "textausgabe.h"
#include "vp.h"
#include "cache.h"
#include "kernel.h"

//Systemzeiger
extern struct Window *fenster;
extern struct Screen *schirm;

//Maus
extern WORD MausX;
extern WORD MausY;

//Programmsystemvariablen
extern FLOAT frame20;
extern BOOL erreicht;
extern BOOL hauptsichtbar;
char hauptipe[31] = "Hauptperson";

//Datenstrukturen
extern struct ORT ort;
struct ILK ilk;
extern struct NICHTSPROG nichtsprog;

//Elementzeiger
struct PERSON *rootperson = NULL;

//Aktuelles Objekt
extern char aktbez[61];
extern UWORD aktid;
extern UWORD akttyp;
extern void *aktzeig;

//Aktionen
extern UWORD benutzt;
extern UWORD invbenutzt;
extern UWORD angesehen;

/*=========================Personen Objekte====================*/
/*----------Datenverwaltung----------*/
void LadeLaufkarte(STRPTR datei) {
	char datilk[100];
	UWORD i;
	struct CACHE *file;
	STRPTR p;

	strcpy(datilk, "Dats/"); strcat(datilk, datei); strcat(datilk, ".ilk");

	if (!(file = COpen(datilk))) {
		if (p = strrchr(datilk, '_')) {
			strcpy(p, ".ilk");
			file = COpen(datilk);
		}
	}
	if (file) {
		CRead(file, &ilk, sizeof(struct ILK));
		CClose(file);
	} else {
		for (i = 0; i < 640; i++) {
			ilk.oben[i] = 1; ilk.unten[i] = 239;
		}
	}
}

void LaufkarteLinie(UBYTE p, WORD x1, WORD y1, WORD x2, WORD y2) {
	WORD tw;
	WORD z;
	WORD br;
	FLOAT dif;

	if (x1 > x2) {
		tw = x1; x1 = x2; x2 = tw;
		tw = y1; y1 = y2; y2 = tw;
	}
	y1 = y1 >> 1; y2 = y2 >> 1;

	br = x2 - x1;
	if (br != 0) {
		dif = (y2 - y1) / (FLOAT)br;
		for (z = 0; z <= br; z++) {
			if (p == 0) {
				ilk.oben[x1 + z]=(UBYTE)(y1 + (dif * z));
			} else {
				ilk.unten[x1 + z]=(UBYTE)(y1 + (dif * z));
			}
		}
	}
}

struct IPE *LadeIPE(STRPTR datei) {
	struct IPE *root = NULL;
	struct IPE *alt;
	struct IPE *neu;
	struct DIPE dipe;
	char datipe[100];
	struct CACHE *file;
	UWORD i;
	UWORD anz;

	strcpy(datipe, "Dats/"); strcat(datipe, datei); strcat(datipe, ".ipe");
	if (file = COpen(datipe)) {
		CRead(file, &anz, sizeof(UWORD));
		if (anz > 0) {
			if (root = malloc(sizeof(IPE))) {
				CRead(file, &dipe, sizeof(DIPE));
				root->id = dipe.id;
				root->ri = dipe.ri;
				strcpy(root->datei, dipe.datei);
				root->ian = NULL;
				root->next = NULL;
				alt = root;
				for (i = 1; i < anz; i++) {
					if (neu = malloc(sizeof(IPE))) {
						CRead(file, &dipe, sizeof(DIPE));
						neu->id = dipe.id;
						neu->ri = dipe.ri;
						strcpy(neu->datei, dipe.datei);
						neu->ian = NULL;
						neu->next = NULL;
						alt->next = neu;
						alt = neu;
					} else Fehler(0, "ipe");
				}
			}
		}
		CClose(file);
	} else Fehler(2, datipe);
	return(root);
}

void EntferneIPE(struct IPE *ipe) {
	struct IPE *akt;
	struct IPE *next;

	akt = ipe;
	while (akt) {
		next = akt->next;
		EntferneIAN(akt->ian);
		free(akt);
		akt = next;
	}
}

void SetzeIPEIAN(struct PERSON *pers, UWORD id) {
	struct IPE *akt;
	struct IPE *iperi[4] = {NULL, NULL, NULL, NULL};
	struct IPE *ipe = NULL;

	if (!((pers->animid == id) && (pers->animri == pers->richtung)))  {
		pers->animnum = 0; pers->animp = 0;
		pers->animid = id; pers->animri = pers->richtung;
		akt = pers->ipe;
		while (akt) {
			if ((akt->id == id) && (akt->ri == pers->richtung)) {
				ipe = akt;
				break;
			}
			if (akt->id == id) iperi[akt->ri] = akt;
			akt = akt->next;
		}
		if (!ipe) {
			if (((ULONG)iperi[0] + (ULONG)iperi[1] + (ULONG)iperi[2] + (ULONG)iperi[3]) == NULL) {
				ipe = pers->ipe;
				printf("SetzeIPEIAN Fehler:%ld\n", id);
			} else {
				if (pers->richtung == RI_LINKS) {
					ipe = iperi[RI_VORNE];
					if (!ipe) ipe = iperi[RI_HINTEN];
					if (!ipe) ipe = iperi[RI_RECHTS];
				}
				if (pers->richtung == RI_RECHTS) {
					ipe = iperi[RI_VORNE];
					if (!ipe) ipe = iperi[RI_HINTEN];
					if (!ipe) ipe = iperi[RI_LINKS];
				}
				if ((pers->richtung == RI_VORNE) || (pers->richtung == RI_HINTEN)) {
					if (pers->x < 320) {
						ipe = iperi[RI_LINKS];
					} else {
						ipe = iperi[RI_RECHTS];
					}
					if (!ipe) ipe = iperi[RI_HINTEN];
					if (!ipe) ipe = iperi[RI_VORNE];
					if (!ipe) ipe = iperi[RI_LINKS];
					if (!ipe) ipe = iperi[RI_RECHTS];
				}
			}
		}
		if (!ipe->ian) ipe->ian = LadeIAN(ipe->datei, MASKE_ERSTELLEN);
		pers->ian = ipe->ian;
	}
}

struct PERSON *SucheIDPerson(UWORD id) {
	struct PERSON *akt;

	akt = rootperson;
	while (akt) {
		if (akt->id == id) return(akt);
		akt = akt->next;
	}
	Fehler(6, "Ungültige Personen-ID");
}

void Vorladen(UWORD id, UWORD iannum) {
	struct PERSON *person;
	struct IPE *ipe;

	if (person = SucheIDPerson(id)) {
		ipe = person->ipe;
		while (ipe) {
			if ((ipe->id == iannum) && !ipe->ian) ipe->ian = LadeIAN(ipe->datei, MASKE_ERSTELLEN);
			ipe = ipe->next;
		}
	} else Fehler(6, "Personen-ID ungültig");
}

void Freigeben(UWORD id, UWORD iannum) {
	struct PERSON *person;
	struct IPE *ipe;

	if (person = SucheIDPerson(id)) {
		ipe = person->ipe;
		while (ipe) {
			if ((ipe->id == iannum) && !ipe->ian) {
				EntferneIAN(ipe->ian);
				ipe->ian = NULL;
			}
			ipe = ipe->next;
		}
	} else Fehler(6, "Personen-ID ungültig");
}

struct PERSON *AddPerson(UWORD id, WORD x, WORD y, STRPTR datei, STRPTR name) {
	struct PERSON *neu;

	if (neu = malloc(sizeof(PERSON))) {
		neu->x = x; neu->y = y;
		neu->id = id;
		strncpy(neu->name, name, 61);
		neu->altx = 0; neu->alty = 0; neu->altbreite = 0; neu->althoehe = 0;
		neu->sichtbar = TRUE;
		neu->vgmaske = TRUE;
		neu->lauffaktor = 12;
		neu->standiannum = 1;
		neu->p1 = 0; neu->p2 = 0; neu->p3 = 0; neu->p4 = 0; neu->aktion = AKT_NICHTS;
		neu->ian = NULL; neu->animid = 0;
		neu->animnum = 0; neu->animp = 0;
		neu->richtung = RI_VORNE;
		neu->isfaktiv = FALSE;
		neu->pc = 0;
		neu->ipe = LadeIPE(datei);
		SetzeIPEIAN(neu, neu->standiannum);

		neu->next = rootperson;
		rootperson = neu;
	} else Fehler(0, datei);
	return(neu)
}

void EntfernePerson(UWORD id) {
	struct PERSON *akt;

	if (id == 0) hauptsichtbar=FALSE; else SetzeSicht(ort.id, id, FALSE);

	akt = rootperson;
	while (akt) {
		if (akt->id == id) {
			akt->sichtbar = FALSE;
			break;
		}
		akt = akt->next;
	}
}

void EntferneAllePersonen(BOOL haupt) {
	struct PERSON *next;
	struct PERSON *akt;

	akt = rootperson;
	while (akt) {
		next = akt->next;
		if (!haupt && (akt->id == 0)) {
			rootperson = akt;
			akt->next = NULL;
		} else {
			EntferneIPE(akt->ipe);
			free(akt);
		}
		akt = next;
	}
	if (haupt) rootperson = NULL;
}

void ErsetzeHauptpersonIPE(STRPTR datei) {
	struct PERSON *akt;
	struct PERSON *alt;

	if (stricmp(datei, hauptipe) != 0) {
		akt = rootperson; alt = NULL;
		while (akt) {
			if (akt->id == 0) {
				akt->aktion = AKT_NICHTS;
				EntferneIPE(akt->ipe);
				akt->ipe = LadeIPE(datei);
				akt->animid = 0;
				SetzeIPEIAN(akt, akt->standiannum);
				strcpy(hauptipe, datei);
				break;
			}
			alt = akt; akt = akt->next;
		}
	}
}

void PersonRichtung(UWORD id, WORD x, WORD y) {
	struct PERSON *akt;

	akt = SucheIDPerson(id);
	x = x - (WORD)akt->x; y = y - (WORD)akt->y;
	if (abs((LONG)y) >= abs((LONG)x)) {
		if (y >= 0) akt->richtung = RI_VORNE;
		if (y < 0) akt->richtung = RI_HINTEN;
	} else {
		if (x >= 0) akt->richtung = RI_RECHTS;
		if (x < 0) akt->richtung = RI_LINKS;
	}
}

void PersonenAktion(UWORD id, UWORD aktion, UWORD p1, UWORD p2, UWORD p3, UWORD p4) {
	struct PERSON *akt;

	akt = SucheIDPerson(id);
	akt->aktion = aktion;
	akt->p1 = p1;
	akt->p2 = p2;
	akt->p3 = p3;
	akt->p4 = p4;
	if ((aktion == AKT_LAUFEN) && (p4 != 1)) {
		if (ilk.unten[p1] > ilk.oben[p1]) {
			if (p2 > (ilk.unten[p1] << 1)) akt->p2 = ilk.unten[p1] << 1;
			if (p2 < (ilk.oben[p1] << 1)) akt->p2 = ilk.oben[p1] << 1;
		} else akt->p2 = ilk.oben[p1] + ilk.unten[p1];
	}
}

void SortierePersonen() {
	struct PERSON *akt;
	struct PERSON *buf1;
	struct PERSON *buf2;
	struct PERSON *alt;

	alt = NULL; akt = rootperson;
	while (akt && akt->next) {
		if (akt->y > akt->next->y) {
			buf1 = akt->next;
			akt->next = akt->next->next;
			if (alt) {
				buf2 = alt->next;
				alt->next = buf1;
			} else {
				buf2 = rootperson;
				rootperson = buf1;
			}
			buf1->next = buf2;
			alt = NULL; akt = rootperson;
		} else {
			alt = akt; akt = akt->next;
		}
	}
}

/*---------Grafikanzeige---------*/
void BltTestePersonen() {
	struct PERSON *akt;
	WORD gpx;
	WORD gpy;
	FLOAT vx;
	FLOAT vy;
	FLOAT nv;
	FLOAT tp;
	BOOL wieder;
	FLOAT lauffaktor;

	akt = rootperson;
	while (akt) {
		akt->altx = (WORD)akt->x - akt->ian->greifpx;
		akt->alty = (WORD)akt->y - akt->ian->greifpy;
		akt->altbreite = akt->ian->breite;
		akt->althoehe = akt->ian->hoehe;

		if (akt->isfaktiv) {
			do {
				akt->pc = LaufeINGA(akt->pc, &wieder);
			} while (wieder);
		}

		if ((akt->id == 0) && (akt->aktion != AKT_NICHTS)) nichtsprog.frame = 0;

		switch (akt->aktion) {
			case AKT_NICHTS:
				SetzeIPEIAN(akt, akt->standiannum);
			break;

			case AKT_LAUFEN:
				vx = (akt->p1 - akt->x); vy = (akt->p2 - akt->y); nv = sqrt((vx * vx) + (vy * vy));
				lauffaktor = frame20 * akt->lauffaktor;
				if (lauffaktor > 40) lauffaktor = 40;
				if (nv > lauffaktor) {
					nv = nv / lauffaktor;
				} else {
					if (nv > 0) nv = 1;
				}

				if (nv >= 1) {
					tp = akt->x + (vx / nv);
					if ((ilk.oben[(WORD)tp] > ilk.unten[(WORD)tp]) && (akt->p4 != 1)) {
						akt->aktion = AKT_NICHTS;
						if ((benutzt + angesehen > 0) && (akt->p4 == 2) && (akt->id == 0)) {
							erreicht = TRUE;
						} else {
							benutzt = 0; invbenutzt = 0; angesehen = 0;
						}
					} else {
						akt->x = tp; akt->y += (vy / nv);
						if (akt->p4 != 1) {
							if ((akt->y) > (ilk.unten[(WORD)akt->x] * 2)) akt->y = ilk.unten[(WORD)akt->x] * 2;
							if ((akt->y) < (ilk.oben[(WORD)akt->x] * 2)) akt->y = ilk.oben[(WORD)akt->x] * 2;
						}

						if (fabs(vy) >= fabs(vx)) {
							if (vy >= 0) akt->richtung = RI_VORNE;
							if (vy < 0) akt->richtung = RI_HINTEN;
						} else {
							if (vx <= 0) akt->richtung = RI_LINKS;
							if (vx > 0) akt->richtung = RI_RECHTS;
						}
						SetzeIPEIAN(akt, akt->p3);
					}
				} else {
					akt->aktion = AKT_NICHTS;
					if ((benutzt + angesehen > 0) && (akt->id == 0)) erreicht = TRUE;
				}
			break;

			case AKT_REDEN:
				SetzeIPEIAN(akt, akt->p1);
			break;

			case AKT_ANIM:
				if (akt->p2 > 0) {
					SetzeIPEIAN(akt, akt->p1);
				} else {
					akt->aktion = AKT_NICHTS;
				}
			break;

			case AKT_NEHMEN:
				if (akt->p1 != 0) {
					SetzeIPEIAN(akt, akt->p2);
				} else {
					akt->aktion = AKT_NICHTS;
				}
			break;
		}

		if (akt->sichtbar) {

			akt->animp += frame20;
			if (akt->animp >= akt->ian->ppf + 1) {
				if ((akt->aktion == AKT_NEHMEN) && (akt->animnum + 1 == akt->p3)) EntferneObjekt(akt->p1);
				akt->animnum++;
				if (akt->animnum >= akt->ian->frames) {
					akt->animnum = 0;
					if (akt->aktion == AKT_ANIM) {
						akt->p2--; if (akt->p2 == 0) akt->animnum = akt->ian->frames - 1;
					}
					if (akt->aktion == AKT_NEHMEN) {
						akt->p1 = 0; akt->animnum = akt->ian->frames - 1;
					}
				}
				akt->animp = 0;
			}

			BltIANFrame(akt->ian, akt->animnum, (WORD)akt->x, (WORD)akt->y, akt->vgmaske);

			gpx = (WORD)akt->x - akt->ian->greifpx;
			gpy = (WORD)akt->y - akt->ian->greifpy;
			if ((MausX >= gpx) && (MausX <= gpx + akt->ian->breite) && (MausY >= gpy) && (MausY <= gpy + akt->ian->hoehe)) {
				if (akt->name[0] && (akt->id > aktid)) {
					strcpy(aktbez, akt->name);
					aktid = akt->id;
					akttyp = 3;
					aktzeig = (void *)akt;
				}
			}
		}

		akt = akt->next;
	}
}

void BltPersonenWeg() {
	struct PERSON *akt;
	WORD gpx;
	WORD gpy;
	UWORD hohe;

	akt = rootperson;
	while (akt) {
		gpx = akt->altx;
		gpy = akt->alty;

		hohe = akt->althoehe;
		if (gpy + akt->althoehe > 479) {
			hohe = 480 - gpy;
		} else if (gpy < 0) {
			hohe = hohe + gpy;
			gpy = 0;
		}
		BltBitMapRastPort(ort.ibm->bild, gpx, gpy, fenster->RPort, gpx, gpy, akt->altbreite, hohe, 192);
		akt = akt->next;
	}
}
