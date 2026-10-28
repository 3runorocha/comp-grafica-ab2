#ifndef CALIB_H
#define CALIB_H

#include "camera.h"
#include <GLFW/glfw3.h>

// modo de calibração das câmeras

#define TILE_SIZE 0.5f   // azulejo em metros

// cria a grade e os eixos
void calib_init(void);
// move a câmera pelo teclado
void calib_update(GLFWwindow *win, CameraShot *shot, float dt);
// desenha eixos e grade
void calib_draw(const mat4 *view, const mat4 *proj);

// alterna grade diagonal
void calib_toggle_diagonal(void);
// grade está diagonal
int calib_is_diagonal(void);

#endif
