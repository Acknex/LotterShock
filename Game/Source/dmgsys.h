#ifndef DMGSYS_H
#define DMGSYS_H

#define DMGSYS_NONE        0 //! Keine Schadensquelle
#define DMGSYS_ENVIRONMENT 1 //! Umgebungsschaden (Säure o.ä.)
#define DMGSYS_PLAYER      2 //! Quelle = Spieler
#define DMGSYS_ENEMY       3 //! Quelle = Gegner

//! @param src Herkunft des Schadens
//! @param ctx Kontext-Entity, falls Quelle benötigt wird
//! @param dmg Menge des Schadens
void dmgsys_set_src(int src, ENTITY * ctx, var dmg);

//! Fragt die Schadensquelle ab
int dmgsys_get_src();

//! Fragt die Herkunfts-Entity ab
ENTITY * dmgsys_get_ctx();

//! Fragt den Schaden ab
var dmgsys_get_dmg();

#endif // DMGSYS_H
