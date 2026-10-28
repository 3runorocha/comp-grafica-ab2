#ifndef ROOM_H
#define ROOM_H

#include "mathlib.h"

// limites e colisão da sala

// carrega limites e móveis da sala
int room_load(const char *path);

// colisão com móveis e paredes
void room_collide(vec3 *pos, float radius);

// registra obstáculo da cena
int room_add_obstacle(const char *name, float xmin, float xmax, float zmin, float zmax, float height);

// move obstáculo registrado
void room_move_obstacle(int index, float xmin, float xmax, float zmin, float zmax);

// desenha o arame de debug
void room_draw_debug(const mat4 *view, const mat4 *proj);

#endif
