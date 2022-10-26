#ifndef LAUNCH_HEAVY_1_H
#define LAUNCH_HEAVY_1_H

#include "quantum.h"

#define xxx KC_NO

#define LAYOUT( \
 K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C,  K0D,  K0E,  K0F, K0G, K0H, K0I, \
 K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C,  K1D,  K1E,  K1F, K1G, K1H, K1I, \
  K20,  K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D, K2E,  K2F, K2G, K2H, K2I, \
   K30,  K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B,  K3C,    K3D,  K3E, K3F, K3G,      \
     K40,  K41, K42, K43, K44, K45, K46, K47, K48, K49, K4A,  K4B,  K4C,       K4D, K4E, K4F, K4G, \
   K50, K51, K52, K53,    K54,      K55,  K56, K57, K58,       K59, K5A, K5B,     K5C,   K5D       \
) { \
    { K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D }, \
    { K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D }, \
    { K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D }, \
    { K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C, K0E }, \
    { K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K4A, K4B, K4C, K1E }, \
    { K50, K51, K52, K53, K54, K3D, K55, K56, K57, K58, K59, K5A, K5B, K2E }, \
    { xxx, xxx, xxx, K0I, K0H, K0G, xxx, xxx, xxx, xxx, xxx, xxx, K0F, xxx }, \
    { xxx, xxx, xxx, K1I, K1H, K1G, xxx, xxx, xxx, xxx, xxx, xxx, K1F, xxx }, \
    { xxx, xxx, xxx, K2I, K2H, K2G, xxx, xxx, xxx, xxx, xxx, xxx, K2F, xxx }, \
    { xxx, xxx, xxx, xxx, K3G, K3F, xxx, xxx, xxx, xxx, xxx, xxx, K3E, xxx }, \
    { xxx, xxx, xxx, K4G, K4F, K4E, xxx, xxx, xxx, xxx, xxx, xxx, K4D, xxx }, \
    { xxx, xxx, xxx, xxx, K5D, xxx, xxx, xxx, xxx, xxx, xxx, xxx, K5C, xxx }, \
}

#endif // LAUNCH_HEAVY_1_H
