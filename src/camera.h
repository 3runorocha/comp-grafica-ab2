#ifndef CAMERA_H
#define CAMERA_H

#include <glad/gl.h>
#include "mathlib.h"
#include "renderer.h"

// ângulo de câmera fixo com fundo
typedef struct {
    char name[32];
    char bg_path[256];
    GLuint bg_tex;
    vec3 eye;       // posição em metros
    float yaw;      // graus em torno de y
    float pitch;    // graus, positivo para cima
    float fov;      // fov vertical em graus
    SceneLight light;   // luz de base
} CameraShot;

#define MAX_SHOTS 8

// direção da câmera
vec3 shot_forward(const CameraShot *s);
// matriz de visão
mat4 shot_view(const CameraShot *s);
// matriz de projeção
mat4 shot_projection(const CameraShot *s, float aspect);

// carrega os ângulos de câmera
int shots_load(const char *path, CameraShot *out, int max);
// salva os ângulos de câmera
int shots_save(const char *path, const CameraShot *shots, int count);
// libera as texturas de fundo
void shots_free(CameraShot *shots, int count);

// carrega a luz de cada câmera
void shots_load_lights(const char *path, CameraShot *shots, int count);

#endif
