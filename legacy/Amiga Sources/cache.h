// Prototypen
void EntleereCache();
BOOL CacheAufraumen();
struct CACHE *COpen(STRPTR datei);
void CClose(struct CACHE *akt);
LONG CRead(struct CACHE *akt, APTR puf, LONG len);
APTR CPtr(struct CACHE *akt, LONG len);
