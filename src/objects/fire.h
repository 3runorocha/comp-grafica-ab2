#ifndef OBJECTS_FIRE_H
#define OBJECTS_FIRE_H

#include "../node.h"
#include "../renderer.h"

// fogo da lareira com chamas
typedef struct {
    Node *root;
    int lit;
    float growth;     // 0 apagado, 1 fogo cheio
    float time;
    float flicker;
} Fire;

// cria a lareira
void fire_create(Fire *f, vec3 pos, float yaw_deg);
// acende/apaga o fogo
void fire_toggle(Fire *f);
// anima o fogo
void fire_update(Fire *f, float dt);
// luz pontual do fogo
int fire_light(const Fire *f, PointLight *out);

// desenha as chamas
void fire_draw_flames(const Fire *f, const mat4 *view, const mat4 *proj);

// ponto de interação
vec3 fire_spot(const Fire *f);

#endif
