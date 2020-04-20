// Prototypen
struct IPE *LadeIPE(STRPTR datei);
void EntferneIPE(struct IPE *ipe);
void SetzeIPEIAN(struct PERSON *pers, UWORD id);
void Vorladen(UWORD id, UWORD iannum);
void Freigeben(UWORD id, UWORD iannum);
struct PERSON *AddPerson(UWORD id, WORD x, WORD y, STRPTR datei, STRPTR name);
void EntfernePerson(UWORD id);
void EntferneAllePersonen(BOOL haupt);
void BltTestePersonen();
void BltPersonenWeg();
struct PERSON *SucheIDPerson(UWORD id);
void ErsetzeHauptpersonIPE(STRPTR datei);
void PersonRichtung(UWORD id, WORD x, WORD y);
void SortierePersonen();
void PersonenAktion(UWORD id, UWORD aktion, UWORD p1, UWORD p2, UWORD p3, UWORD p4);
void LadeLaufkarte(STRPTR datei);
void LaufkarteLinie(UBYTE p, WORD x1, WORD y1, WORD x2, WORD y2);
