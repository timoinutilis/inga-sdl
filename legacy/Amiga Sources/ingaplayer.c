#include <stdio.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/medplayer.h>

#include <exec/exec.h>
#include <libraries/medplayer.h>

#include "kernel.h"

//Systemzeiger
struct Library *MEDPlayerBase = NULL;

//Player intern
struct MMD0 *module = NULL;
ULONG error = 1;
BOOL spiele = FALSE;
char lied[31] = "";
UBYTE modnummer;


//---Verwaltung------

void LadePlayer() {
	if (MEDPlayerBase=OpenLibrary("medplayer.library", 6)) {
		error = GetPlayer(0);
	} else Meldung("Konnte medplayer.library nicht öffnen.\nMusik ist deaktiviert.");
}

void EntfernePlayer() {
	if (MEDPlayerBase) {
		if (spiele) StopPlayer();
		FreePlayer();
		CloseLibrary(MEDPlayerBase);
	}
}

//---Befehle--------

void PlSpieleModule(STRPTR datei, UBYTE num, UBYTE anfang) {
	char dat[50];

	if (!error) {
		if (module) {
			if (stricmp(datei, lied) == 0) {
				if ((modnummer != num) || !spiele || (anfang == 1)) {
					if (spiele) StopPlayer();
					modnummer = num; spiele = TRUE;
					SetModnum(modnummer); PlayModule(module);
				}
			} else {
				if (spiele) {
					StopPlayer(); spiele = FALSE;
				}
				UnLoadModule(module); module = NULL;
			}
		}
		if (!module) {
			strcpy(dat, "MED/");
			strcat(dat, datei);
			strcpy(lied, datei);
			modnummer = num;
			if (module = LoadModule(dat)) {
				SetModnum(modnummer);
				PlayModule(module);
				spiele = TRUE;
			} else printf("Could not open module\n");
		};
	};
}

void PlStoppeModule() {
	if (spiele && !error) {
		StopPlayer();
		spiele = FALSE;
	}
}

void PlModuleWeiter() {
	if (module && !spiele && !error) {
		ContModule(module);
		spiele = TRUE;
	}
}
