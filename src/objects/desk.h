#ifndef OBJECTS_DESK_H
#define OBJECTS_DESK_H

#include "../node.h"

// escrivaninha com gaveta deslizante
#define DESK_W 1.60f   // comprimento da mesa
#define DESK_D 0.60f   // profundidade da mesa
#define DRAWER_W 0.44f // largura da gaveta
#define DRAWER_X 0.40f // centro da gaveta

typedef struct {
    Node *root;
    Node *drawer;     // grupo da gaveta
    float open;       // 0 fechada, 1 aberta
    float target;
} Desk;

// cria a escrivaninha
void desk_create(Desk *d, vec3 pos, float yaw_deg);
// abre/fecha a gaveta
void desk_toggle_drawer(Desk *d);
// anima a gaveta
void desk_update(Desk *d, float dt);

// posição da frente da gaveta
vec3 desk_handle_pos(const Desk *d);

#endif
