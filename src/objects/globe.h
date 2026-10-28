#ifndef OBJECTS_GLOBE_H
#define OBJECTS_GLOBE_H

#include "../node.h"

// globo terrestre giratório
typedef struct {
    Node *root;
    Node *spin;       // grupo que gira
    float speed;      // graus por segundo
} Globe;

// cria o globo
void globe_create(Globe *g, vec3 pos, float yaw_deg);
// impulso de giro
void globe_push(Globe *g);
// anima o giro
void globe_update(Globe *g, float dt);

#endif
