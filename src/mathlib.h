#ifndef MATHLIB_H
#define MATHLIB_H

// matemática 3d (vetores e matrizes)

// tipos vetor e matriz coluna-major
typedef struct { float x, y, z; } vec3;
typedef struct { float m[16]; } mat4;

#define PI_F 3.14159265358979f

// graus para radianos
static inline float deg2rad(float d) { return d * PI_F / 180.0f; }

// operações com vetores
vec3  v3(float x, float y, float z);
vec3  v3_add(vec3 a, vec3 b);
vec3  v3_sub(vec3 a, vec3 b);
vec3  v3_scale(vec3 a, float s);
float v3_dot(vec3 a, vec3 b);
vec3  v3_cross(vec3 a, vec3 b);
float v3_length(vec3 a);
vec3  v3_normalize(vec3 a);

// matriz identidade
mat4 m4_identity(void);
// produto a * b
mat4 m4_mul(mat4 a, mat4 b);
// matriz de translação
mat4 m4_translate(vec3 t);
// matriz de escala
mat4 m4_scale(vec3 s);
// rotação em torno de um eixo
mat4 m4_rotate(float angle_rad, vec3 axis);
// projeção perspectiva
mat4 m4_perspective(float fovy_rad, float aspect, float znear, float zfar);
// matriz de visão da câmera
mat4 m4_lookat(vec3 eye, vec3 center, vec3 up);

#endif
