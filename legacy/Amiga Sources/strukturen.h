#include <dos/dos.h>
#include <exec/exec.h>

// Mauszeiger
#define MAUS_ZEIGER 0
#define MAUS_ECKE 1
#define MAUS_WARTE 2
#define MAUS_UNSICHTBAR 3

// Maskenart
#define MASKE_KEINE 0
#define MASKE_DATEI 1
#define MASKE_ERSTELLEN 2

// Richtungen
#define RI_VORNE 2
#define RI_HINTEN 3
#define RI_LINKS 0
#define RI_RECHTS 1

// Aktionen
#define AKT_NICHTS 0
#define AKT_LAUFEN 1
#define AKT_REDEN 2
#define AKT_ANIM 3
#define AKT_NEHMEN 4
// 0=Nichts (Steh-Animation)
// 1=Laufen
//    p1,p2=Zielposition
//    p3=ianNum
//    p4=0: Gehkarte wird beachtet
//    p4=1: Direkter Weg
//    p4=2: Gehkarte, so weit, wie möglich
// 2=Reden
//    p1=ianNum
// 3=Animation abspielen
//    p1=ianNum
//    p2=Anzahl
// 4=Gegenstand nehmen
//    p1=id.
//    p2=ianNum
//    p3=Framenummer bei Nehmen.

// BitMap
struct IBM {
	ULONG kopf;
	UWORD breite;
	UWORD hoehe;
	UBYTE tiefe;
	UWORD bpr;
	UBYTE flags;
	ULONG reserv1, reserv2;
	struct BitMap *bild;
	struct BitMap *maske;
};

// Animationsdaten
struct IAN {
	UWORD frames;
	UWORD richtung; /* 0=horizontal 1=vertikal */
	UWORD breite;
	UWORD hoehe;
	UWORD greifpx;
	UWORD greifpy;
	UWORD ppf; /*Pausen pro Frame*/
	ULONG reserv1, reserv2;
	struct IBM *ibm;
};

// MaskPlane
struct IMP {
	ULONG kopf;
	UWORD breite;
	UWORD hoehe;
	ULONG bytes;
};

// Bezeichnungstext
struct BEZ {
	WORD x;
	WORD y;
	WORD len;
	WORD altx;
	WORD alty;
	WORD altlen;
};

// Einfaches Feld zum Anwählen
struct FELD {
	WORD lox; /*Ecke links oben*/
	WORD loy;
	WORD rux; /*Ecke rechts unten*/
	WORD ruy;
	UWORD id;
	WORD gehx; /*Laufzielpunkt*/
	WORD gehy;
	BOOL sichtbar;
	char name[61];
	struct FELD *next;
};

// Animationselement ohne Funktion
struct ZIERDE {
	WORD x;
	WORD y;
	UWORD id;
	UWORD animnum; /*aktuelles Frame der Animation*/
	FLOAT animp; /*Pausenzähler der Animation*/
	BOOL sichtbar;
	struct IAN *ian;
	struct ZIERDE *next;
};

// Animationselement zum Anwählen
struct OBJEKT {
	WORD x;
	WORD y;
	UWORD id;
	char name[61];
	WORD gehx;
	WORD gehy;
	UWORD animnum;
	FLOAT animp;
	BOOL standbild;
	BOOL sichtbar;
	struct IAN *ian;
	struct OBJEKT *next;
};

// Laufkarte
struct ILK {
	UBYTE oben[640];
	UBYTE unten[640];
};

// Knoten einer Personenliste
struct IPE {
	UWORD id;
	UBYTE ri;
	char datei[31];
	struct IAN *ian;
	struct IPE *next;
};

// Dateisatz einer Personenliste
struct DIPE {
	UWORD id;
	UBYTE ri;
	char datei[31];
};

// Person mit Steuerungsdaten
struct PERSON {
	FLOAT x;
	FLOAT y;
	UWORD id;
	char name[61];
	WORD altx;
	WORD alty;
	UWORD altbreite;
	UWORD althoehe;
	BOOL sichtbar;
	BOOL vgmaske; // Vordergrund-Maske benutzen
	UBYTE lauffaktor;
	UBYTE standiannum;
	UWORD p1; // Aktionsparameter
	UWORD p2;
	UWORD p3;
	UWORD p4;
	UWORD aktion; // siehe unten
	UWORD richtung; // 2=Vorne 3=Hinten 0=Links 1=Rechts
	struct IAN *ian; // Aktuelle IAN
	UWORD animid; // Aktuelle AnimationsID aus der IPE-Liste
	UBYTE animri; // Aktuelle Richtung der Animation
	UWORD animnum;
	FLOAT animp;
	BOOL isfaktiv; // Skriptbearbeitung aktiv
	ULONG pc; // Programmzähler für Steuerungsskript bei NSCs
	struct IPE *ipe;
	struct PERSON *next;
};

// Struktur für NichtsProg
struct NICHTSPROG {
	FLOAT frame;
	UWORD dauer;
	ULONG ptr;
};

// Variablenstruktur
struct VARIABLE {
	UWORD vid;
	UWORD wert;
	struct VARIABLE *next;
};

// Sichtbarkeitsstruktur
struct SICHT {
	UBYTE ortid;
	UBYTE elemid;
	BOOL sichtbar;
	struct SICHT *next;
};

// Ortdaten
struct ORT {
	struct IBM *ibm;
	UWORD id;
	ULONG ptr;
};

// Einstellungen und Daten der Sprach(text)ausgabe
struct SPRECH {
	char text[101];
	FLOAT zeitz;
	WORD pers;
	WORD x;
	WORD y;
	FLOAT tempo;
	BOOL sound;
	BOOL spracheaktiv;
	BOOL untertitel;
};

// Einblendung des aktiven InvGegs
struct INVBEZ{
	WORD x, y;
	WORD altx, alty;
	UWORD animnum;
	FLOAT animp;
};

// Inventargegenstand
struct INVGEG {
	struct IAN *ian;
	UWORD id;
	char name[61];
	char datei[31];
	struct INVGEG *next;
};

// Inventarleiste
struct INVBAR {
	struct IBM *ibm;
	struct IAN *ian;
	BOOL aktiv;
	BOOL sperre;
	struct INVGEG *erster;
	UWORD pos;
	UWORD animnum[8]; /*aktuelles Frame der Animation*/
	FLOAT animp[8]; /*Pausenzähler der Animation*/
};

// Antwort
struct ANTWORT {
	UBYTE aid;
	char text[101];
	struct ANTWORT *next;
};

// Dialog
struct DIALOG {
	UBYTE anz;
	BOOL aktiv;
};

// Menübildschirm
struct MENU {
    struct IBM *bild;
    struct IAN *punkt;
    struct IBM *prozessleer;
    struct IBM *prozessvoll;
};

//Spielzeit
struct SPIELZEIT {
	FLOAT sekunden;
	UWORD stunden;
};

// Spielstand
struct SAVE {
	ULONG ortptr;
	WORD x;
	WORD y;
	FLOAT sprechtempo;
	BOOL untertitel;
	BOOL spracheaktiv;
	struct SPIELZEIT zeit;
	char hauptipe[31];
	UWORD anzvar;
	UWORD anzsicht;
	UWORD anzgeg;
};

// Gecachte Datei
struct CACHE {
	char name[100];
	BOOL offen;
	LONG pos;
	LONG grosse;
	APTR data;
	UWORD alter;
	struct CACHE *next;
};

// Basiswerte für das Sound-Plug-In
struct SOUNDBASE {
	BOOL soundplugin; // TRUE, wenn Sound-Plug-In aktiv
	BOOL speech; // TRUE, wenn gerade Sprache abgespielt wird. Danach wieder FALSE
	BPTR conwin; // Zeiger auf DOS-Ausgabefenster
};

// Sound-Plug-In Message.
struct SOUNDMESSAGE {
	struct Message ExecMessage;
	BOOL busy;
	BOOL cancel;
	UBYTE action;
	char file[256];
	UWORD id;
	UWORD vol;
	UWORD pan; 
	struct SOUNDBASE *soundbase;
};
