#include "crate.h"
#include "../materials.h"
#include <math.h>

#define POST   0.05f    // seção dos montantes
#define SLAT_T 0.02f    // espessura das ripas
#define SLATS  4        // ripas por lado
#define GAP    0.018f   // fresta entre ripas

// monta o caixote
void crate_create(Crate *c, vec3 pos, float yaw_deg)
{
    const float s = CRATE_SIZE, h = s * 0.5f;
    Node *root = c->root = node_group(NULL, pos);
    root->rot.y = yaw_deg;

    // miolo escuro
    node_box(root, v3(s - 0.08f, s - 0.06f, s - 0.08f), v3(0, h, 0), &mat_shadow);

    // montantes dos cantos
    float pc = h - POST * 0.5f;
    for (int i = 0; i < 4; i++) {
        float x = (i & 1) ? pc : -pc, z = (i & 2) ? pc : -pc;
        node_box(root, v3(POST, s - SLAT_T, POST), v3(x, (s - SLAT_T) * 0.5f, z), &mat_wood_raw);
    }

    // ripas laterais
    float slat_len = s - 2 * POST;
    float slat_h = (s - SLAT_T - (SLATS + 1) * GAP) / SLATS;
    float face = h - SLAT_T * 0.5f - 0.004f;
    for (int k = 0; k < SLATS; k++) {
        float y = GAP + slat_h * 0.5f + k * (slat_h + GAP);
        node_box(root, v3(slat_len, slat_h, SLAT_T), v3(0, y, face), &mat_wood_raw);
        node_box(root, v3(slat_len, slat_h, SLAT_T), v3(0, y, -face), &mat_wood_raw);
        node_box(root, v3(SLAT_T, slat_h, slat_len), v3(face, y, 0), &mat_wood_raw);
        node_box(root, v3(SLAT_T, slat_h, slat_len), v3(-face, y, 0), &mat_wood_raw);
    }

    // tampa de tábuas
    const int boards = 4;
    float board_w = (s - (boards - 1) * 0.012f) / boards;
    for (int k = 0; k < boards; k++) {
        float z = -h + board_w * 0.5f + k * (board_w + 0.012f);
        node_box(root, v3(s, SLAT_T, board_w), v3(0, s - SLAT_T * 0.5f, z), &mat_wood_raw);
    }

    // diagonais de reforço
    float diag = sqrtf(2.0f) * slat_len - 0.06f;
    for (int side = -1; side <= 1; side += 2) {
        Node *b = node_box(root, v3(diag, 0.07f, SLAT_T), v3(0, h, side * (face + SLAT_T)), &mat_wood_raw);
        b->rot.z = 45.0f * side;
    }
}
