#ifndef MESH_H
#define MESH_H

#include <glad/gl.h>
#include "mathlib.h"

// malha na gpu (vao + vbo)
typedef struct {
    GLuint vao, vbo;
    GLsizei count;   // número de vértices
    GLenum mode;     // triângulos ou linhas
} Mesh;

// cria malha iluminada (pos, normal, uv)
Mesh mesh_create_pnu(const float *verts, int count);
// cria malha de linhas coloridas
Mesh mesh_create_lines(const float *verts, int count);
// desenha a malha
void mesh_draw(const Mesh *m);
// libera a malha
void mesh_destroy(Mesh *m);

// caixa centrada na origem
Mesh mesh_box(vec3 size, float tex_meters);

// cubo unitário
Mesh mesh_cube(void);

// cilindro no eixo y
Mesh mesh_cylinder(float radius, float height, int segments, float tex_meters);

// toro no plano xz
Mesh mesh_torus(float major_r, float minor_r, int seg_major, int seg_minor, float tex_meters);

// superfície de revolução de um perfil
Mesh mesh_lathe(const float *profile, int n, int segments, float u_repeat, float v_repeat);

// esfera com uv de mapa-múndi
Mesh mesh_sphere(float radius, int segments, int rings);

#endif
