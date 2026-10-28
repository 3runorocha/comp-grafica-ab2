#ifndef OBJECTS_LAMP_H
#define OBJECTS_LAMP_H

#include "../node.h"
#include "../renderer.h"

// abajur de mesa
typedef struct {
    Node *root;
    int on;
} Lamp;

// cria o abajur
void lamp_create(Lamp *l, vec3 pos, float yaw_deg);
// liga/desliga o abajur
void lamp_toggle(Lamp *l);
// atualiza o brilho
void lamp_update(Lamp *l, float dt);

// luz pontual do abajur
int lamp_light(const Lamp *l, PointLight *out);

#endif
