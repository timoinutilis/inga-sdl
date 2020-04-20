#include <stdio.h>

#include <exec/exec.h>
#include <libraries/cdplay.h>
#include <dos/dos.h>
#include <devices/timer.h>

#include <proto/exec.h>
#include <proto/cdplay.h>
#include <proto/dos.h>
#include <proto/timer.h>

#include "kernel.h"
#include "device_info.h"

//Systemzeiger
struct Library *CDPlayBase = NULL;
struct CDRequest *cdreq = NULL;

//Programmsystemvariablen
extern FLOAT frame20;
char cddrive[30] = "CD0:";
extern struct timeval systime;

//CD intern
UWORD cdtrack = 0;
struct timeval cdtrende;


/*=======================================================*/

void StarteAudioCDTreiber() {
	struct device_info_s *dev;
	//Treiber starten und alles initialisieren.

	if (CDPlayBase = OpenLibrary("cdplay.library", 0)) {	
		if (dev = get_device_info(cddrive)) {
			cdreq = CDOpenDrive(dev->device, dev->unit);
			free_device_info(dev);
		}
		if (!cdreq) Meldung("Entweder liegt keine CD im Laufwerk,\noder die Piktogramm-Eigenschaften\nsind flasch konfiguriert.\nCD-Musik ist deaktiviert.");
	} else Meldung("Konnte cdplay.library nicht öffnen.\nCD-Musik ist deaktiviert.");
}

void EntferneAudioCDTreiber() {
	//Erst überprüfen, ob Treiber überhaupt gestartet ist!
	
	if (cdreq) {
		CDStop(cdreq);
		CDCloseDrive(cdreq);
	}

	CloseLibrary(CDPlayBase);

}

void TesteAudioCD() {
	//Falls Lied beendet, neu beginnen! (repeat)

	if (cdreq) {
		if (CmpTime(&cdtrende, &systime) == 1) {
			CDPlay((ULONG)cdtrack, cdreq);
			CDUpdate(cdreq);
			GetSysTime(&cdtrende);
			cdtrende.tv_secs += (ULONG)(cdreq->cdr_Time->cdt_TrackRemainBase / 75) - 2;
		}
	}
}

//========Befehle===========

void SpieleCDTrack(UWORD num) {
	if (num != cdtrack) {
		cdtrack = num;
	
		//Starte Lied von CD!
		if (cdreq) {
			CDPlay((ULONG)cdtrack, cdreq);
			CDUpdate(cdreq);
			GetSysTime(&cdtrende);
			cdtrende.tv_secs += (ULONG)(cdreq->cdr_Time->cdt_TrackRemainBase / 75) - 2;
		}
	}
}

void StoppeCD() {
	if (cdtrack > 0) {
	
		//Stoppe CD!
		if (cdreq) CDStop(cdreq);
		cdtrack = 0;
	}
}
