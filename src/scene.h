#ifndef SCENE_H
#define SCENE_H

#include "mathlib.h"

// objetos da lesson room

// cria os objetos
void scene_init(void);
// anima os objetos
void scene_update(float dt);
// desenha os objetos
void scene_draw(void);

// desenha as chamas
void scene_draw_behind(const mat4 *view, const mat4 *proj);

// desenha as sombras no chão
void scene_draw_shadows(const mat4 *view, const mat4 *proj, vec3 player_pos);

// interage com o objeto mais próximo
void scene_interact(vec3 pos);

#endif
