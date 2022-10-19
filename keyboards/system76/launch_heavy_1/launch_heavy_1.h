#ifndef LAUNCH_HEAVY_1_H
#define LAUNCH_HEAVY_1_H

#include "quantum.h"

#define xxx KC_NO

#define LAYOUT( \
 K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C,  K0D,  K0E,  K6B, K65, K64, K63, \
 K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C,  K1D,  K1E,  K7B, K75, K74, K73, \
  K20,  K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D, K2E,  K8B, K85, K84, K83, \
   K30,  K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B,  K3C,    K3D,  K9B, K95, K94,      \
     K40,  K41, K42, K43, K44, K45, K46, K47, K48, K49, K4A,  K4B,  K4C,       KAB, KA5, KA4, KA3, \
   K50, K51, K52, K53,    K54,      K55,  K56, K57, K58,       K59, K5A, K5B,     KBB,   KB4       \
) { \
    { K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D }, \
    { K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D }, \
    { K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D }, \
    { K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C, K0E }, \
    { K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K4A, K4B, K4C, K1E }, \
    { K50, K51, K52, K53, K54, K3D, K55, K56, K57, K58, K59, K5A, K5B, K2E }, \
    { xxx, xxx, xxx, K63, K64, K65, xxx, xxx, xxx, xxx, xxx, xxx, K6B, xxx }, \
    { xxx, xxx, xxx, K73, K74, K75, xxx, xxx, xxx, xxx, xxx, xxx, K7B, xxx }, \
    { xxx, xxx, xxx, K83, K84, K85, xxx, xxx, xxx, xxx, xxx, xxx, K8B, xxx }, \
    { xxx, xxx, xxx, xxx, K94, K95, xxx, xxx, xxx, xxx, xxx, xxx, K9B, xxx }, \
    { xxx, xxx, xxx, KA3, KA4, KA5, xxx, xxx, xxx, xxx, xxx, xxx, KAB, xxx }, \
    { xxx, xxx, xxx, xxx, KB4, xxx, xxx, xxx, xxx, xxx, xxx, xxx, KBB, xxx }, \
}

#endif // LAUNCH_HEAVY_1_H
