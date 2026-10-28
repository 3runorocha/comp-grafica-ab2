#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>
#include "mathlib.h"

// carrega e liga o shader
GLuint shader_load(const char *vs_path, const char *fs_path);

// envia uniforms ao shader
void shader_set_mat4(GLuint prog, const char *name, const mat4 *m);
void shader_set_vec3(GLuint prog, const char *name, vec3 v);
void shader_set_float(GLuint prog, const char *name, float f);
void shader_set_int(GLuint prog, const char *name, int i);

#endif
