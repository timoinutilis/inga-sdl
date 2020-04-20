// Prototypen
struct BitMap *LadeIMP(STRPTR datei);
struct IBM *LadeIBM(STRPTR datei, WORD maske);
void EntferneIBMIMP(struct IBM *ibm);
void BildWechsel();
void FadeIn(UBYTE tempo);
void FadeOut(UBYTE tempo);
void ZeigeBild(struct IBM *ibm);
void LadeHintergrund(STRPTR datei);
void RestauriereHintergrund(WORD x, WORD y, UWORD breite, UWORD hoehe);
void Restauration();
void EntferneAlleRestaurationen();
