#include "node.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_POOL_SIZE 512
#define TEX_METERS 0.5f   // textura a cada 50 cm

// pool fixo de nós
static Node pool[NODE_POOL_SIZE];
static int pool_used;

// aloca um nó e liga ao pai
static Node *node_alloc(Node *parent, vec3 pos)
{
    if (pool_used == NODE_POOL_SIZE) {
        fprintf(stderr, "Erro: NODE_POOL_SIZE esgotado\n");
        exit(1);
    }
    Node *n = &pool[pool_used++];
    memset(n, 0, sizeof *n);
    n->pos = pos;
    n->scale = v3(1, 1, 1);
    // adiciona como filho
    if (parent) {
        if (parent->child_count == NODE_MAX_CHILDREN) {
            fprintf(stderr, "Erro: NODE_MAX_CHILDREN esgotado\n");
            exit(1);
        }
        parent->children[parent->child_count++] = n;
    }
    return n;
}

// nó agrupador
Node *node_group(Node *parent, vec3 pos)
{
    return node_alloc(parent, pos);
}

// peça caixa
Node *node_box(Node *parent, vec3 size, vec3 pos, const Material *mat)
{
    Node *n = node_alloc(parent, pos);
    n->mesh = mesh_box(size, TEX_METERS);
    n->mat = mat;
    return n;
}

// peça cilindro
Node *node_cylinder(Node *parent, float radius, float height, vec3 pos, const Material *mat)
{
    Node *n = node_alloc(parent, pos);
    n->mesh = mesh_cylinder(radius, height, 24, TEX_METERS);
    n->mat = mat;
    return n;
}

// peça toro
Node *node_torus(Node *parent, float major_r, float minor_r, vec3 pos, const Material *mat)
{
    Node *n = node_alloc(parent, pos);
    n->mesh = mesh_torus(major_r, minor_r, 32, 12, TEX_METERS);
    n->mat = mat;
    return n;
}

// peça torneada
Node *node_lathe(Node *parent, const float *profile, int n, vec3 pos, const Material *mat)
{
    // raio máximo e altura do perfil
    float rmax = 0.0f;
    for (int i = 0; i < n; i++)
        rmax = fmaxf(rmax, profile[2 * i]);
    float height = fabsf(profile[2 * n - 1] - profile[1]);
    Node *nd = node_alloc(parent, pos);
    nd->mesh = mesh_lathe(profile, n, 32, fmaxf(1.0f, roundf(2.0f * PI_F * rmax / TEX_METERS)),
                          fmaxf(0.25f, height / TEX_METERS));
    nd->mat = mat;
    return nd;
}

// peça esfera
Node *node_sphere(Node *parent, float radius, vec3 pos, const Material *mat)
{
    Node *n = node_alloc(parent, pos);
    n->mesh = mesh_sphere(radius, 48, 24);
    n->mat = mat;
    return n;
}

// caixa com textura inteira por face
Node *node_box_fit(Node *parent, vec3 size, vec3 pos, const Material *mat)
{
    Node *n = node_alloc(parent, pos);
    n->mesh = mesh_box(size, 0.0f);
    n->mat = mat;
    return n;
}

// aponta o eixo y na direção d
static void point_y_axis_along(Node *n, vec3 d)
{
    n->rot.x = atan2f(sqrtf(d.x * d.x + d.z * d.z), d.y) * 180.0f / PI_F;
    n->rot.y = atan2f(d.x, d.z) * 180.0f / PI_F;
}

// cilindro entre dois pontos
Node *node_cylinder_between(Node *parent, vec3 a, vec3 b, float radius, const Material *mat)
{
    vec3 d = v3_sub(b, a);
    Node *n = node_cylinder(parent, radius, v3_length(d), v3_scale(v3_add(a, b), 0.5f), mat);
    point_y_axis_along(n, d);
    return n;
}

// barra entre dois pontos
Node *node_box_between(Node *parent, vec3 a, vec3 b, float thickness, const Material *mat)
{
    vec3 d = v3_sub(b, a);
    Node *n = node_box(parent, v3(thickness, v3_length(d), thickness), v3_scale(v3_add(a, b), 0.5f), mat);
    point_y_axis_along(n, d);
    return n;
}

// nó com malha pronta
Node *node_mesh(Node *parent, Mesh mesh, vec3 pos, const Material *mat)
{
    Node *n = node_alloc(parent, pos);
    n->mesh = mesh;
    n->mat = mat;
    return n;
}

// pendura um nó no pai
void node_attach(Node *parent, Node *child)
{
    if (parent->child_count == NODE_MAX_CHILDREN) {
        fprintf(stderr, "Erro: NODE_MAX_CHILDREN esgotado\n");
        exit(1);
    }
    parent->children[parent->child_count++] = child;
}

// matriz local t * r * s
mat4 node_local_matrix(const Node *n)
{
    mat4 r = m4_mul(m4_rotate(deg2rad(n->rot.y), v3(0, 1, 0)),
             m4_mul(m4_rotate(deg2rad(n->rot.x), v3(1, 0, 0)),
                    m4_rotate(deg2rad(n->rot.z), v3(0, 0, 1))));
    return m4_mul(m4_translate(n->pos), m4_mul(r, m4_scale(n->scale)));
}

// desenha o nó e os filhos
void node_draw(const Node *n, const mat4 *parent_world)
{
    mat4 local = node_local_matrix(n);
    mat4 world = parent_world ? m4_mul(*parent_world, local) : local;

    // desenha a própria malha
    if (n->mesh.count > 0 && n->mat)
        renderer_draw(&n->mesh, &world, n->mat);

    // desenha os filhos
    for (int i = 0; i < n->child_count; i++)
        node_draw(n->children[i], &world);
}
