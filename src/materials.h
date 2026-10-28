#ifndef MATERIALS_H
#define MATERIALS_H

#include "renderer.h"

// materiais da cena
extern Material mat_wood_varnish;   // madeira envernizada
extern Material mat_wood_raw;       // madeira crua
extern Material mat_wood_inner;     // madeira interna escura
extern Material mat_brass;          // latão
extern Material mat_shadow;         // fundo de vãos
extern Material mat_cloth;          // roupa
extern Material mat_metal_painted;  // metal pintado
extern Material mat_steel;          // aço
extern Material mat_rubber;         // borracha
extern Material mat_wood_chair;     // madeira da cadeira
extern Material mat_leather;        // couro
extern Material mat_wood_easel;     // madeira do cavalete
extern Material mat_slate;          // lousa
extern Material mat_chalk;          // giz
extern Material mat_felt;           // feltro do apagador
extern Material mat_brass_aged;     // latão envelhecido
extern Material mat_globe_map;      // mapa do globo
extern Material mat_ceramic;        // cerâmica do abajur
extern Material mat_lampshade;      // cúpula do abajur
extern Material mat_bulb;           // lâmpada
extern Material mat_log;            // lenha carbonizada
extern Material mat_coal;           // carvão em brasa
extern Material mat_iron;           // ferro da grelha

// carrega as texturas dos materiais
void materials_init(void);

#endif
