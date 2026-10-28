#ifndef CHARACTER_H
#define CHARACTER_H

#include "node.h"

// personagem importado montado em nós

#define CHAR_MAX_PARTS 24

// partes do personagem
typedef struct {
    Node *root;
    Node *body;
    Node *parts[CHAR_MAX_PARTS];
    int part_count;
    Material mat;
} Character;

// carrega o modelo, esqueleto e textura
int character_load(Character *c, const char *dir, float scale, float facing_deg);

#endif
