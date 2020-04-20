// Prototypen
void LadeINGA(void);
void EntferneINGA(void);
ULONG LaufeINGA(ULONG ptr, BOOL *wieder);
UWORD VarWert(UWORD vid);
UWORD VarInit(UWORD vid, UWORD wert);
void SetzeVar(UWORD vid, UWORD wert);
void EntferneAlleVariablen();
UWORD VarAnzahl();
BOOL SichtWert(UBYTE ortid, UBYTE elemid);
BOOL SichtInit(UBYTE ortid, UBYTE elemid, BOOL wert);
void SetzeSicht(UBYTE ortid, UBYTE elemid, BOOL wert);
void EntferneAlleSichts();
UWORD SichtAnzahl();
