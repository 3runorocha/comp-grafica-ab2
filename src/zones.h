#ifndef ZONES_H
#define ZONES_H

#include "camera.h"

// troca automática de câmera por zonas

// carrega as zonas do arquivo
int zones_load(const char *path, const CameraShot *shots, int shot_count);

// câmera ativa pela posição
int zones_check(int current, vec3 pos);

// desenha as zonas no chão
void zones_draw_debug(const mat4 *view, const mat4 *proj, int current);

#endif
