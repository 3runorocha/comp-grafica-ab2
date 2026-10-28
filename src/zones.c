#include "zones.h"
#include "mesh.h"
#include "shader.h"
#include <stdio.h>
#include <string.h>

#define MAX_ZONES 32

// zona de troca de câmera
typedef struct {
    int from, to;          // índices das câmeras
    float x[4], z[4];      // cantos no chão
} CameraZone;

static CameraZone zones[MAX_ZONES];
static int zone_count;
static GLuint line_prog;
static Mesh zone_lines[MAX_ZONES];

// índice da câmera pelo nome
static int shot_index(const CameraShot *shots, int count, const char *name)
{
    for (int i = 0; i < count; i++)
        if (strcmp(shots[i].name, name) == 0)
            return i;
    return -1;
}

// contorno de debug da zona
static void build_debug_mesh(int i)
{
    // arestas do quadrilátero
    float v[8 * 6];
    float *p = v;
    for (int k = 0; k < 4; k++) {
        int n = (k + 1) % 4;
        float pts[2][3] = { { zones[i].x[k], 0.01f, zones[i].z[k] }, { zones[i].x[n], 0.01f, zones[i].z[n] } };
        for (int e = 0; e < 2; e++) {
            *p++ = pts[e][0]; *p++ = pts[e][1]; *p++ = pts[e][2];
            *p++ = 1.0f; *p++ = 1.0f; *p++ = 1.0f;
        }
    }
    // recria a malha
    if (zone_lines[i].vao)
        mesh_destroy(&zone_lines[i]);
    zone_lines[i] = mesh_create_lines(v, 8);
}

// carrega as zonas do arquivo
int zones_load(const char *path, const CameraShot *shots, int shot_count)
{
    if (!line_prog)
        line_prog = shader_load("shaders/lines.vert", "shaders/zone.frag");
    zone_count = 0;

    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Aviso: sem zonas de camera (%s)\n", path);
        return 0;
    }
    // lê cada zona
    char line[512];
    while (zone_count < MAX_ZONES && fgets(line, sizeof line, f)) {
        if (line[0] == '#')
            continue;
        char from[32], to[32];
        CameraZone *z = &zones[zone_count];
        if (sscanf(line, "%31s %31s %f %f %f %f %f %f %f %f", from, to,
                   &z->x[0], &z->z[0], &z->x[1], &z->z[1], &z->x[2], &z->z[2], &z->x[3], &z->z[3]) != 10)
            continue;
        z->from = shot_index(shots, shot_count, from);
        z->to = shot_index(shots, shot_count, to);
        if (z->from < 0 || z->to < 0)
            continue;
        build_debug_mesh(zone_count);
        zone_count++;
    }
    fclose(f);
    printf("%d zonas de troca de camera carregadas de %s\n", zone_count, path);
    return zone_count;
}

// ponto dentro do quadrilátero
static int inside(const CameraZone *z, float px, float pz)
{
    int in = 0;
    for (int i = 0, j = 3; i < 4; j = i++) {
        if ((z->z[i] > pz) != (z->z[j] > pz)) {
            float x_cross = z->x[i] + (pz - z->z[i]) * (z->x[j] - z->x[i]) / (z->z[j] - z->z[i]);
            if (px < x_cross)
                in = !in;
        }
    }
    return in;
}

// câmera ativa pela posição
int zones_check(int current, vec3 pos)
{
    for (int i = 0; i < zone_count; i++)
        if (zones[i].from == current && inside(&zones[i], pos.x, pos.z))
            return zones[i].to;
    return current;
}

// desenha as zonas no chão
void zones_draw_debug(const mat4 *view, const mat4 *proj, int current)
{
    if (!line_prog)
        return;
    glDisable(GL_DEPTH_TEST);
    glUseProgram(line_prog);
    mat4 vp = m4_mul(*proj, *view);
    shader_set_mat4(line_prog, "u_vp", &vp);
    // cor por zona
    for (int i = 0; i < zone_count; i++) {
        vec3 color = zones[i].from == current ? v3(0.1f, 1.0f, 0.3f) : v3(0.8f, 0.2f, 0.2f);
        shader_set_vec3(line_prog, "u_color", color);
        mesh_draw(&zone_lines[i]);
    }
    glEnable(GL_DEPTH_TEST);
}
