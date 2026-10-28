#include "room.h"
#include "mesh.h"
#include "shader.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define MAX_BOXES 16

// obstáculo em caixa no chão
typedef struct {
    char name[32];
    float xmin, xmax, zmin, zmax, height;
} Box;

static float xmin = -4, xmax = 4, zmin = -6, zmax = 6, height = 3.5f;
static Box boxes[MAX_BOXES];        // móveis do fundo
static int box_count;
static Box obstacles[MAX_BOXES];    // objetos 3d da cena
static int obstacle_count;
static int loaded;
static GLuint prog;
static Mesh wire;

// adiciona um vértice colorido
static float *push(float *p, float x, float y, float z, vec3 color)
{
    *p++ = x; *p++ = y; *p++ = z;
    *p++ = color.x; *p++ = color.y; *p++ = color.z;
    return p;
}

// arestas de uma caixa
static float *push_box(float *p, float x0, float x1, float z0, float z1, float h, vec3 color)
{
    float xs[4] = { x0, x1, x1, x0 }, zs[4] = { z0, z0, z1, z1 };
    for (int i = 0; i < 4; i++) {
        int j = (i + 1) % 4;
        p = push(p, xs[i], 0.005f, zs[i], color); p = push(p, xs[j], 0.005f, zs[j], color);
        p = push(p, xs[i], h, zs[i], color);      p = push(p, xs[j], h, zs[j], color);
        p = push(p, xs[i], 0.0f, zs[i], color);   p = push(p, xs[i], h, zs[i], color);
    }
    return p;
}

// monta o arame de debug
static void build_wire(void)
{
    static float v[(2 * MAX_BOXES + 1) * 24 * 6];
    float *p = push_box(v, xmin, xmax, zmin, zmax, height, v3(0.2f, 1.0f, 1.0f));   // sala: ciano
    for (int i = 0; i < box_count; i++)
        p = push_box(p, boxes[i].xmin, boxes[i].xmax, boxes[i].zmin, boxes[i].zmax, boxes[i].height,
                     v3(1.0f, 0.4f, 1.0f));                                            // móveis: magenta
    for (int i = 0; i < obstacle_count; i++)
        p = push_box(p, obstacles[i].xmin, obstacles[i].xmax, obstacles[i].zmin, obstacles[i].zmax,
                     obstacles[i].height, v3(1.0f, 0.6f, 0.1f));                       // objetos 3d: laranja
    // recria a malha
    if (wire.vao)
        mesh_destroy(&wire);
    wire = mesh_create_lines(v, (box_count + obstacle_count + 1) * 24);
}

// registra obstáculo da cena
int room_add_obstacle(const char *name, float x0, float x1, float z0, float z1, float h)
{
    if (obstacle_count == MAX_BOXES)
        return -1;
    Box *b = &obstacles[obstacle_count];
    snprintf(b->name, sizeof b->name, "%s", name);
    b->xmin = x0; b->xmax = x1; b->zmin = z0; b->zmax = z1; b->height = h;
    if (prog)
        build_wire();
    return obstacle_count++;
}

// move obstáculo registrado
void room_move_obstacle(int index, float x0, float x1, float z0, float z1)
{
    if (index < 0 || index >= obstacle_count)
        return;
    Box *b = &obstacles[index];
    b->xmin = x0; b->xmax = x1; b->zmin = z0; b->zmax = z1;
    if (prog)
        build_wire();
}

// carrega limites e móveis da sala
int room_load(const char *path)
{
    if (!prog)
        prog = shader_load("shaders/lines.vert", "shaders/lines.frag");
    loaded = 0;
    box_count = 0;
    FILE *f = fopen(path, "r");
    // lê caixas e limites
    if (f) {
        char line[256];
        while (fgets(line, sizeof line, f)) {
            Box b;
            if (line[0] == '#')
                continue;
            if (strncmp(line, "box", 3) == 0) {
                if (box_count < MAX_BOXES &&
                    sscanf(line, "box %31s %f %f %f %f %f", b.name, &b.xmin, &b.xmax, &b.zmin, &b.zmax, &b.height) == 6)
                    boxes[box_count++] = b;
            } else if (!loaded && sscanf(line, "%f %f %f %f %f", &xmin, &xmax, &zmin, &zmax, &height) == 5) {
                loaded = 1;
            }
        }
        fclose(f);
    }
    if (!loaded)
        fprintf(stderr, "Aviso: limites da sala nao lidos de %s (usando padrao)\n", path);
    build_wire();
    return loaded;
}

// colisão círculo contra caixa
static void collide_box(vec3 *pos, float radius, const Box *b)
{
    float cx = fmaxf(b->xmin, fminf(b->xmax, pos->x));
    float cz = fmaxf(b->zmin, fminf(b->zmax, pos->z));
    float dx = pos->x - cx, dz = pos->z - cz;
    float d2 = dx * dx + dz * dz;

    // centro fora: empurra para fora
    if (d2 > 1e-8f) {
        if (d2 < radius * radius) {
            float d = sqrtf(d2);
            pos->x = cx + dx / d * radius;
            pos->z = cz + dz / d * radius;
        }
        return;
    }
    // centro dentro: sai pelo lado mais próximo
    float left = pos->x - b->xmin, right = b->xmax - pos->x;
    float back = pos->z - b->zmin, front = b->zmax - pos->z;
    float m = fminf(fminf(left, right), fminf(back, front));
    if (m == left)       pos->x = b->xmin - radius;
    else if (m == right) pos->x = b->xmax + radius;
    else if (m == back)  pos->z = b->zmin - radius;
    else                 pos->z = b->zmax + radius;
}

// colisão com móveis e paredes
void room_collide(vec3 *pos, float radius)
{
    for (int i = 0; i < box_count; i++)
        collide_box(pos, radius, &boxes[i]);
    for (int i = 0; i < obstacle_count; i++)
        collide_box(pos, radius, &obstacles[i]);
    // limita às paredes
    pos->x = fmaxf(xmin + radius, fminf(xmax - radius, pos->x));
    pos->z = fmaxf(zmin + radius, fminf(zmax - radius, pos->z));
}

// desenha o arame de debug
void room_draw_debug(const mat4 *view, const mat4 *proj)
{
    glDisable(GL_DEPTH_TEST);
    glUseProgram(prog);
    mat4 vp = m4_mul(*proj, *view);
    shader_set_mat4(prog, "u_vp", &vp);
    mesh_draw(&wire);
    glEnable(GL_DEPTH_TEST);
}
