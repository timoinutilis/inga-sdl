#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <clib/alib_protos.h>

#include <exec/exec.h>
#include <dos/dos.h>
#include <workbench/icon.h>

#include "strukturen.h"
#include "ingasound.h"
#include "kernel.h"

//Datenstrukturen
struct SOUNDBASE soundbase;

//Sound intern
struct SOUNDMESSAGE smsg;
struct MsgPort *sndport = NULL;


// ---Plug-In-Verwaltung--------

void StarteIngaSound() {
	UBYTE z;
	BPTR file;
	
	if (file = Open("IngaSound", MODE_OLDFILE)) {
		Close(file);
		
		Execute("run >NIL: IngaSound", NULL, NULL);

		Forbid();
		for (z = 0; z < 6; z++) {
			if (sndport = FindPort("IngaSoundPort")) break;
			Delay(20);
		}
		Permit();

		if (sndport) {	
			smsg.ExecMessage.mn_Length = sizeof(SOUNDMESSAGE);
			smsg.soundbase = &soundbase;
			smsg.busy = TRUE;
			smsg.cancel = FALSE;
			smsg.action = 100;
			PutMsg(sndport, (struct Message *)&smsg);
		} else Meldung("Konnte keinen Kontakt zur Sound-Erweiterung herstellen.\nSound ist deaktiviert.");
	} else Meldung("Konnte Sound-Erweiterung nicht öffnen.\nSound ist deaktiviert.");
}

void EntferneIngaSound() {
	if (sndport) {
		while (smsg.busy) Delay(1);
		smsg.busy = TRUE;
		smsg.action = 101;
		PutMsg(sndport, (struct Message *)&smsg);
	}
}

// ---Sprachausgabe------------

void SndSage(STRPTR datei, UWORD x) {
	if (sndport) {
		while (smsg.busy) Delay(1);
		smsg.busy = TRUE;
		smsg.action = 1;
		strcpy(smsg.file, datei);
		smsg.pan = 15 + (UWORD)((FLOAT)x * 0.109375);
		PutMsg(sndport, (struct Message *)&smsg);
	}
}

void SndSageAbbruch() {
	if (sndport) {
		if (smsg.busy) smsg.cancel = TRUE;
	}
}


// ---Soundeffekte-------------

void SndLadeSound(STRPTR datei, UWORD id) {
	if (sndport) {
		while (smsg.busy) Delay(1);
		smsg.busy = TRUE;
		smsg.action = 2;
		strcpy(smsg.file, datei);
		smsg.id = id;
		PutMsg(sndport, (struct Message *)&smsg);
	}
}

void SndEntferneSound(UWORD id) {
	if (sndport) {
		while (smsg.busy) Delay(1);
		smsg.busy = TRUE;
		smsg.action = 3;
		smsg.id = id;
		PutMsg(sndport, (struct Message *)&smsg);
	}
}

void SndSpieleSound(UWORD id, UWORD vol, UWORD pan) {
	if (sndport) {
		while (smsg.busy) Delay(1);
		smsg.busy = TRUE;
		smsg.action = 4;
		smsg.id = id;
		smsg.vol = vol;
		smsg.pan = pan;
		PutMsg(sndport, (struct Message *)&smsg);
	}
}

void SndSpieleSoundSchleife(UWORD id, UWORD vol, UWORD pan) {
	if (sndport) {
		while (smsg.busy) Delay(1);
		smsg.busy = TRUE;
		smsg.action = 5;
		smsg.id = id;
		smsg.vol = vol;
		smsg.pan = pan;
		PutMsg(sndport, (struct Message *)&smsg);
	}
}

void SndSchleifeAbbruch() {
	if (sndport) {
		while (smsg.busy) Delay(1);
		smsg.busy = TRUE;
		smsg.action = 6;
		PutMsg(sndport, (struct Message *)&smsg);
	}
}

void SndVolPan(UWORD vol, UWORD pan) {
	if (sndport) {
		while (smsg.busy) Delay(1);
		smsg.busy = TRUE;
		smsg.action = 7;
		smsg.vol = vol;
		smsg.pan = pan;
		PutMsg(sndport, (struct Message *)&smsg);
	}
}
