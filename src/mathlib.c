#include "mathlib.h"
#include <math.h>

// operações básicas com vetores
vec3 v3(float x, float y, float z) { vec3 r = { x, y, z }; return r; }
vec3 v3_add(vec3 a, vec3 b) { return v3(a.x + b.x, a.y + b.y, a.z + b.z); }
vec3 v3_sub(vec3 a, vec3 b) { return v3(a.x - b.x, a.y - b.y, a.z - b.z); }
vec3 v3_scale(vec3 a, float s) { return v3(a.x * s, a.y * s, a.z * s); }
float v3_dot(vec3 a, vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

// produto vetorial
vec3 v3_cross(vec3 a, vec3 b)
{
    return v3(a.y * b.z - a.z * b.y,
              a.z * b.x - a.x * b.z,
              a.x * b.y - a.y * b.x);
}

// comprimento do vetor
float v3_length(vec3 a) { return sqrtf(v3_dot(a, a)); }

// normaliza o vetor
vec3 v3_normalize(vec3 a)
{
    float len = v3_length(a);
    return len > 0.0f ? v3_scale(a, 1.0f / len) : a;
}

// matriz identidade
mat4 m4_identity(void)
{
    mat4 r = { { 0 } };
    r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
    return r;
}

// produto de matrizes
mat4 m4_mul(mat4 a, mat4 b)
{
    mat4 r;
    for (int c = 0; c < 4; c++) {
        for (int l = 0; l < 4; l++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++)
                sum += a.m[k * 4 + l] * b.m[c * 4 + k];
            r.m[c * 4 + l] = sum;
        }
    }
    return r;
}

// matriz de translação
mat4 m4_translate(vec3 t)
{
    mat4 r = m4_identity();
    r.m[12] = t.x;
    r.m[13] = t.y;
    r.m[14] = t.z;
    return r;
}

// matriz de escala
mat4 m4_scale(vec3 s)
{
    mat4 r = m4_identity();
    r.m[0] = s.x;
    r.m[5] = s.y;
    r.m[10] = s.z;
    return r;
}

// rotação em torno de um eixo
mat4 m4_rotate(float angle_rad, vec3 axis)
{
    vec3 a = v3_normalize(axis);
    float c = cosf(angle_rad), s = sinf(angle_rad), t = 1.0f - c;
    mat4 r = m4_identity();

    r.m[0] = t * a.x * a.x + c;
    r.m[1] = t * a.x * a.y + s * a.z;
    r.m[2] = t * a.x * a.z - s * a.y;

    r.m[4] = t * a.x * a.y - s * a.z;
    r.m[5] = t * a.y * a.y + c;
    r.m[6] = t * a.y * a.z + s * a.x;

    r.m[8]  = t * a.x * a.z + s * a.y;
    r.m[9]  = t * a.y * a.z - s * a.x;
    r.m[10] = t * a.z * a.z + c;
    return r;
}

// projeção perspectiva
mat4 m4_perspective(float fovy_rad, float aspect, float znear, float zfar)
{
    float f = 1.0f / tanf(fovy_rad * 0.5f);
    mat4 r = { { 0 } };
    r.m[0] = f / aspect;
    r.m[5] = f;
    r.m[10] = (zfar + znear) / (znear - zfar);
    r.m[11] = -1.0f;
    r.m[14] = (2.0f * zfar * znear) / (znear - zfar);
    return r;
}

// matriz de visão da câmera
mat4 m4_lookat(vec3 eye, vec3 center, vec3 up)
{
    vec3 f = v3_normalize(v3_sub(center, eye));
    vec3 s = v3_normalize(v3_cross(f, up));
    vec3 u = v3_cross(s, f);
    mat4 r = m4_identity();

    r.m[0] = s.x;  r.m[4] = s.y;  r.m[8]  = s.z;
    r.m[1] = u.x;  r.m[5] = u.y;  r.m[9]  = u.z;
    r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z;

    r.m[12] = -v3_dot(s, eye);
    r.m[13] = -v3_dot(u, eye);
    r.m[14] = v3_dot(f, eye);
    return r;
}
