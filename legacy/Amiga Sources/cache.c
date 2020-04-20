#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dos/dos.h>
#include <exec/exec.h>

#include <proto/dos.h>
#include <proto/exec.h>

#include "strukturen.h"
#include "cache.h"

struct CACHE *rootcache = NULL;
ULONG minfree = 2097152; // Mindestens 2 MB RAM frei halten

/*===========================================*/
// Löscht alle Dateien aus dem Cache.
void EntleereCache() {
	struct CACHE *akt;
	struct CACHE *next;
	
	akt = rootcache;
	while (akt) {
		next = akt->next;
		free(akt->data);
		free(akt);
		akt = next;
	}
}

// Löscht die älteste Datei aus dem Cache.
BOOL CacheAufraumen() {
	struct CACHE *akt;
	struct CACHE *vor;
	struct CACHE *altester;
	struct CACHE *voraltester;
	UWORD alter;
	
	akt = rootcache; vor = NULL;
	altester = NULL; voraltester = NULL;
	alter = 0;
	while (akt) {
		if (!akt->offen && (akt->alter >= alter)) {
			altester = akt; voraltester = vor;
			alter = akt->alter;
		}
		vor = akt;
		akt = akt->next;
	}
	if (altester) {
		if (voraltester) {
			voraltester->next = altester->next;
		} else {
			rootcache = altester->next;
		}
		free(altester->data);
		free(altester);
	} else {
		return(FALSE);
	}
	return(TRUE);
}

// Lädt eine Datei in den Cache.
struct CACHE *COpen(STRPTR datei) {
	struct CACHE *neu = NULL;
	struct CACHE *akt;
	BPTR file;
	
	akt = rootcache;
	while (akt) {
		akt->alter++;
		if (stricmp(datei, akt->name) == 0) {
			akt->offen = TRUE;
			akt->alter = 0;
			neu = akt;
		}
		akt = akt->next;
	}
	if (neu) return(neu);

	if (neu = malloc(sizeof(struct CACHE))) {
		if (file = Open(datei, MODE_OLDFILE)) {
			Seek(file, 0, OFFSET_END);
			neu->grosse = Seek(file, 0, OFFSET_BEGINNING);
			
			while (!(neu->data = malloc(neu->grosse))) {
				if (!CacheAufraumen()) break;
			}
			if (neu->data) {
				strcpy(neu->name, datei);
				neu->offen = TRUE;
				neu->pos = 0;
				neu->alter = 0;
				Read(file, neu->data, neu->grosse);

				neu->next = rootcache;
				rootcache = neu;
			} else {
				free(neu); neu = NULL;
			}
			Close(file);
		} else {
			free(neu); neu = NULL;
		}
	}

	while (AvailMem(MEMF_FAST) < minfree) {
		if (!CacheAufraumen()) break;
	}
	return(neu);
}

// Schließt eine Datei im Cache, so dass sie ggf. daraus gelöscht werden kann.
void CClose(struct CACHE *akt) {
	akt->offen = FALSE;
	akt->pos = 0;
}

// Liest Bytes aus einer Datei im Cache.
LONG CRead(struct CACHE *akt, APTR puf, LONG len) {
	LONG l;
	
	if (akt->pos + len > akt->grosse) l = akt->grosse - akt->pos; else l = len;
	if ((akt->pos % 4 == 0) && ((ULONG)puf % 4 == 0) && (l % 4 == 0)) {
		CopyMemQuick((APTR)((LONG)akt->data + akt->pos), puf, l);
	} else {
		CopyMem((APTR)((LONG)akt->data + akt->pos), puf, l);
	}
	akt->pos = akt->pos + l;
	return(l);
}

// Verschiebt den Lesezeiger einer Datei im Cache.
APTR CPtr(struct CACHE *akt, LONG len) {
	ULONG p;

	p = (ULONG)akt->data + akt->pos;
	akt->pos = akt->pos + len;
	return((APTR)p);
}
