// Prototypen
struct IAN *LadeIAN(STRPTR datei, WORD maske);
void EntferneIAN(struct IAN *ian);
void BltIANFrame(struct IAN *ian, UWORD frame, WORD x, WORD y, BOOL maske);
