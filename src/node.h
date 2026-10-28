#ifndef NODE_H
#define NODE_H

#include "mathlib.h"
#include "mesh.h"
#include "renderer.h"

// grafo de cena hierárquico

#define NODE_MAX_CHILDREN 48

// nó do grafo de cena
typedef struct Node {
    vec3 pos;     // posição relativa ao pai
    vec3 rot;     // rotação em graus (y, x, z)
    vec3 scale;   // escala
    Mesh mesh;    // vazia se só agrupa
    const Material *mat;
    struct Node *children[NODE_MAX_CHILDREN];
    int child_count;
} Node;

// nó agrupador vazio
Node *node_group(Node *parent, vec3 pos);

// peça em forma de caixa
Node *node_box(Node *parent, vec3 size, vec3 pos, const Material *mat);

// peça cilíndrica
Node *node_cylinder(Node *parent, float radius, float height, vec3 pos, const Material *mat);

// toro no plano xz
Node *node_torus(Node *parent, float major_r, float minor_r, vec3 pos, const Material *mat);

// peça torneada por perfil
Node *node_lathe(Node *parent, const float *profile, int n, vec3 pos, const Material *mat);

// esfera com uv de mapa-múndi
Node *node_sphere(Node *parent, float radius, vec3 pos, const Material *mat);

// cilindro ligando dois pontos
Node *node_cylinder_between(Node *parent, vec3 a, vec3 b, float radius, const Material *mat);

// barra quadrada ligando dois pontos
Node *node_box_between(Node *parent, vec3 a, vec3 b, float thickness, const Material *mat);

// caixa com textura inteira por face
Node *node_box_fit(Node *parent, vec3 size, vec3 pos, const Material *mat);

// nó com malha pronta
Node *node_mesh(Node *parent, Mesh mesh, vec3 pos, const Material *mat);

// pendura um nó no pai
void node_attach(Node *parent, Node *child);

// matriz local do nó
mat4 node_local_matrix(const Node *n);
// desenha o nó e os filhos
void node_draw(const Node *n, const mat4 *parent_world);

#endif
