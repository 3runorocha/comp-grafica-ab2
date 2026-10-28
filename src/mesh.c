#include "mesh.h"
#include "mathlib.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

// envia os vértices para a gpu
static Mesh upload(const float *verts, int count, int floats_per_vertex, GLenum mode)
{
    Mesh m = { 0 };
    m.count = count;
    m.mode = mode;
    glGenVertexArrays(1, &m.vao);
    glGenBuffers(1, &m.vbo);
    glBindVertexArray(m.vao);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)count * floats_per_vertex * sizeof(float), verts, GL_STATIC_DRAW);
    return m;
}

// configura um atributo de vértice
static void attrib(GLuint index, int size, int stride_floats, int offset_floats)
{
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride_floats * sizeof(float),
                          (void *)(offset_floats * sizeof(float)));
    glEnableVertexAttribArray(index);
}

// malha com posição, normal e uv
Mesh mesh_create_pnu(const float *verts, int count)
{
    Mesh m = upload(verts, count, 8, GL_TRIANGLES);
    attrib(0, 3, 8, 0);   // posição
    attrib(1, 3, 8, 3);   // normal
    attrib(2, 2, 8, 6);   // uv
    glBindVertexArray(0);
    return m;
}

// malha de linhas com cor
Mesh mesh_create_lines(const float *verts, int count)
{
    Mesh m = upload(verts, count, 6, GL_LINES);
    attrib(0, 3, 6, 0);   // posição
    attrib(1, 3, 6, 3);   // cor
    glBindVertexArray(0);
    return m;
}

// desenha a malha
void mesh_draw(const Mesh *m)
{
    glBindVertexArray(m->vao);
    glDrawArrays(m->mode, 0, m->count);
}

// libera a malha
void mesh_destroy(Mesh *m)
{
    glDeleteBuffers(1, &m->vbo);
    glDeleteVertexArrays(1, &m->vao);
    m->vao = m->vbo = 0;
    m->count = 0;
}

// caixa com textura por metro
Mesh mesh_box(vec3 size, float tex_meters)
{
    // normal e eixos u, v de cada face
    static const float faces[6][3][3] = {
        { {  1, 0, 0 }, { 0, 0, -1 }, { 0, 1,  0 } },
        { { -1, 0, 0 }, { 0, 0,  1 }, { 0, 1,  0 } },
        { {  0, 1, 0 }, { 1, 0,  0 }, { 0, 0, -1 } },
        { {  0,-1, 0 }, { 1, 0,  0 }, { 0, 0,  1 } },
        { {  0, 0, 1 }, { 1, 0,  0 }, { 0, 1,  0 } },
        { {  0, 0,-1 }, {-1, 0,  0 }, { 0, 1,  0 } },
    };
    // cantos dos dois triângulos
    static const float corners[6][2] = {
        { -1, -1 }, { 1, -1 }, { 1, 1 },
        { -1, -1 }, { 1,  1 }, { -1, 1 },
    };
    const float dim[3] = { size.x, size.y, size.z };

    // gera as seis faces
    float verts[36 * 8];
    float *p = verts;
    for (int f = 0; f < 6; f++) {
        const float *n = faces[f][0], *u = faces[f][1], *v = faces[f][2];
        // tamanho da face para a textura
        float ulen = 1.0f, vlen = 1.0f, scale = 1.0f;
        if (tex_meters > 0.0f) {
            ulen = vlen = 0.0f;
            for (int i = 0; i < 3; i++) {
                ulen += fabsf(u[i]) * dim[i];
                vlen += fabsf(v[i]) * dim[i];
            }
            scale = tex_meters;
        }
        // vértices da face
        for (int c = 0; c < 6; c++) {
            float s = corners[c][0], t = corners[c][1];
            for (int i = 0; i < 3; i++)
                *p++ = 0.5f * dim[i] * (n[i] + s * u[i] + t * v[i]);
            for (int i = 0; i < 3; i++)
                *p++ = n[i];
            *p++ = (s + 1.0f) * 0.5f * ulen / scale;
            *p++ = (t + 1.0f) * 0.5f * vlen / scale;
        }
    }
    return mesh_create_pnu(verts, 36);
}

// cubo unitário
Mesh mesh_cube(void)
{
    return mesh_box(v3(1, 1, 1), 1.0f);
}

// escreve um vértice no buffer
static float *push_vertex(float *p, vec3 pos, vec3 n, float u, float v)
{
    *p++ = pos.x; *p++ = pos.y; *p++ = pos.z;
    *p++ = n.x;   *p++ = n.y;   *p++ = n.z;
    *p++ = u;     *p++ = v;
    return p;
}

// cilindro com tampas
Mesh mesh_cylinder(float radius, float height, int segments, float tex_meters)
{
    int count = segments * 12;
    float *verts = malloc((size_t)count * 8 * sizeof(float));
    float *p = verts;
    float h = height * 0.5f;
    float circ = 2.0f * PI_F * radius;
    vec3 up = v3(0, 1, 0), down = v3(0, -1, 0);

    // gera cada segmento
    for (int i = 0; i < segments; i++) {
        float a0 = 2.0f * PI_F * i / segments, a1 = 2.0f * PI_F * (i + 1) / segments;
        vec3 n0 = v3(cosf(a0), 0, sinf(a0)), n1 = v3(cosf(a1), 0, sinf(a1));
        vec3 b0 = v3(n0.x * radius, -h, n0.z * radius), b1 = v3(n1.x * radius, -h, n1.z * radius);
        vec3 t0 = v3(b0.x, h, b0.z), t1 = v3(b1.x, h, b1.z);
        float u0 = circ * i / segments / tex_meters, u1 = circ * (i + 1) / segments / tex_meters;
        float vt = height / tex_meters;

        // lateral
        p = push_vertex(p, b0, n0, u0, 0);
        p = push_vertex(p, t0, n0, u0, vt);
        p = push_vertex(p, b1, n1, u1, 0);
        p = push_vertex(p, b1, n1, u1, 0);
        p = push_vertex(p, t0, n0, u0, vt);
        p = push_vertex(p, t1, n1, u1, vt);

        // tampa de cima
        p = push_vertex(p, v3(0, h, 0), up, 0.5f, 0.5f);
        p = push_vertex(p, t1, up, 0.5f + t1.x / tex_meters, 0.5f + t1.z / tex_meters);
        p = push_vertex(p, t0, up, 0.5f + t0.x / tex_meters, 0.5f + t0.z / tex_meters);

        // tampa de baixo
        p = push_vertex(p, v3(0, -h, 0), down, 0.5f, 0.5f);
        p = push_vertex(p, b0, down, 0.5f + b0.x / tex_meters, 0.5f + b0.z / tex_meters);
        p = push_vertex(p, b1, down, 0.5f + b1.x / tex_meters, 0.5f + b1.z / tex_meters);
    }
    Mesh m = mesh_create_pnu(verts, count);
    free(verts);
    return m;
}

// toro no plano xz
Mesh mesh_torus(float major_r, float minor_r, int seg_major, int seg_minor, float tex_meters)
{
    int count = seg_major * seg_minor * 6;
    float *verts = malloc((size_t)count * 8 * sizeof(float));
    float *p = verts;
    float u_scale = 2.0f * PI_F * major_r / tex_meters, v_scale = 2.0f * PI_F * minor_r / tex_meters;

    // percorre anel e tubo
    for (int i = 0; i < seg_major; i++) {
        for (int j = 0; j < seg_minor; j++) {
            // quatro cantos do quad
            vec3 pos[4], nrm[4];
            float uv[4][2];
            for (int k = 0; k < 4; k++) {
                int ii = i + (k == 2 || k == 3), jj = j + (k == 1 || k == 3);
                float a = 2.0f * PI_F * ii / seg_major, b = 2.0f * PI_F * jj / seg_minor;
                nrm[k] = v3(cosf(b) * cosf(a), sinf(b), cosf(b) * sinf(a));
                pos[k] = v3((major_r + minor_r * cosf(b)) * cosf(a), minor_r * sinf(b),
                            (major_r + minor_r * cosf(b)) * sinf(a));
                uv[k][0] = (float)ii / seg_major * u_scale;
                uv[k][1] = (float)jj / seg_minor * v_scale;
            }
            // dois triângulos do quad
            static const int order[6] = { 0, 1, 2, 2, 1, 3 };
            for (int k = 0; k < 6; k++) {
                int o = order[k];
                p = push_vertex(p, pos[o], nrm[o], uv[o][0], uv[o][1]);
            }
        }
    }
    Mesh m = mesh_create_pnu(verts, count);
    free(verts);
    return m;
}

// superfície de revolução
Mesh mesh_lathe(const float *profile, int n, int segments, float u_repeat, float v_repeat)
{
    // normais e comprimento do perfil
    vec3 *pn = malloc((size_t)n * sizeof(vec3));     // normal por ponto
    float *arc = malloc((size_t)n * sizeof(float));  // comprimento acumulado
    arc[0] = 0.0f;
    for (int j = 0; j < n; j++) {
        vec3 sum = v3(0, 0, 0);
        if (j > 0) {
            float dr = profile[2 * j] - profile[2 * j - 2], dy = profile[2 * j + 1] - profile[2 * j - 1];
            sum = v3_add(sum, v3_normalize(v3(dy, -dr, 0)));
            arc[j] = arc[j - 1] + sqrtf(dr * dr + dy * dy);
        }
        if (j < n - 1) {
            float dr = profile[2 * j + 2] - profile[2 * j], dy = profile[2 * j + 3] - profile[2 * j + 1];
            sum = v3_add(sum, v3_normalize(v3(dy, -dr, 0)));
        }
        pn[j] = v3_normalize(sum);
    }

    // gira o perfil em volta de y
    int count = segments * (n - 1) * 6;
    float *verts = malloc((size_t)count * 8 * sizeof(float));
    float *p = verts;
    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < n - 1; j++) {
            // quatro cantos do quad
            vec3 pos[4], nrm[4];
            float uv[4][2];
            for (int k = 0; k < 4; k++) {
                int ii = i + (k == 2 || k == 3), jj = j + (k == 1 || k == 3);
                float a = 2.0f * PI_F * ii / segments, r = profile[2 * jj], y = profile[2 * jj + 1];
                pos[k] = v3(r * cosf(a), y, r * sinf(a));
                nrm[k] = v3(pn[jj].x * cosf(a), pn[jj].y, pn[jj].x * sinf(a));
                uv[k][0] = (float)(segments - ii) / segments * u_repeat;
                uv[k][1] = arc[jj] / arc[n - 1] * v_repeat;
            }
            // dois triângulos do quad
            static const int order[6] = { 0, 1, 2, 2, 1, 3 };
            for (int k = 0; k < 6; k++)
                p = push_vertex(p, pos[order[k]], nrm[order[k]], uv[order[k]][0], uv[order[k]][1]);
        }
    }
    Mesh m = mesh_create_pnu(verts, count);
    free(verts);
    free(pn);
    free(arc);
    return m;
}

// esfera por revolução de semicírculo
Mesh mesh_sphere(float radius, int segments, int rings)
{
    float *profile = malloc((size_t)(rings + 1) * 2 * sizeof(float));
    for (int j = 0; j <= rings; j++) {
        float lat = -PI_F * 0.5f + PI_F * j / rings;
        profile[2 * j] = radius * cosf(lat);
        profile[2 * j + 1] = radius * sinf(lat);
    }
    Mesh m = mesh_lathe(profile, rings + 1, segments, 1.0f, 1.0f);
    free(profile);
    return m;
}
