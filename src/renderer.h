#ifndef RENDERER_H
#define RENDERER_H

#include <glad/gl.h>
#include "mathlib.h"
#include "mesh.h"

// material phong do objeto
typedef struct {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    GLuint tex;      // textura (0 = sem textura)
    vec3 emissive;   // luz própria
    int additive;    // mistura aditiva
    GLuint emit_tex; // mapa de emissão
} Material;

// luz pontual ou spot
typedef struct {
    vec3 pos;
    vec3 color;     // cor vezes intensidade
    float range;
    vec3 dir;       // direção do cone
    float spot_cos; // -2 = omnidirecional
} PointLight;

#define MAX_POINT_LIGHTS 4   // igual ao lit.frag

// inicia o renderizador
int renderer_init(void);

// luz de base da câmera
typedef struct {
    vec3 dir;          // direção da luz
    vec3 color;        // cor da luz direcional
    vec3 ambient;      // luz ambiente
    float bg_bright;   // brilho do fundo
} SceneLight;

// começa o quadro com câmera e luz
void renderer_begin(const mat4 *view, const mat4 *proj, vec3 eye, const SceneLight *light);

// define as luzes pontuais
void renderer_set_point_lights(const PointLight *lights, int count);

// desenha uma malha com material
void renderer_draw(const Mesh *mesh, const mat4 *model, const Material *mat);

#endif
