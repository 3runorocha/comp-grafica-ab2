#ifndef SHADOW_H
#define SHADOW_H

#include "mathlib.h"

// sombras blob no chão

// carrega shader e retângulo da sombra
int shadow_init(void);

// prepara o estado das sombras
void shadow_begin(const mat4 *view, const mat4 *proj);

// desenha uma sombra elíptica
void shadow_draw(vec3 center, float rx, float rz, float strength);

// restaura o estado
void shadow_end(void);

#endif
