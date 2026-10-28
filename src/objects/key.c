#include "key.h"
#include "../materials.h"

#define RING_R  0.016f    // raio da argola
#define TUBE_R  0.0035f   // espessura da argola
#define Y       TUBE_R    // altura das peças

// monta a chave
void key_create(Key *k, vec3 pos, float yaw_deg)
{
    Node *root = k->root = node_group(NULL, pos);
    root->rot.y = yaw_deg;

    // argola
    node_torus(root, RING_R, TUBE_R, v3(0, Y, 0), &mat_brass_aged);

    // colar e haste
    float x = RING_R + TUBE_R + 0.004f;
    Node *collar = node_cylinder(root, 0.0055f, 0.008f, v3(x, Y, 0), &mat_brass_aged);
    collar->rot.z = 90.0f;
    const float shank = 0.065f;
    Node *shaft = node_cylinder(root, 0.003f, shank, v3(x + 0.004f + shank * 0.5f, Y, 0), &mat_brass_aged);
    shaft->rot.z = 90.0f;
    float tip = x + 0.004f + shank;
    Node *end = node_cylinder(root, 0.004f, 0.004f, v3(tip, Y, 0), &mat_brass_aged);
    end->rot.z = 90.0f;

    // dentes da chave
    node_box(root, v3(0.018f, 0.005f, 0.016f), v3(tip - 0.012f, Y, -0.011f), &mat_brass_aged);
    node_box(root, v3(0.005f, 0.005f, 0.007f), v3(tip - 0.018f, Y, -0.022f), &mat_brass_aged);
    node_box(root, v3(0.005f, 0.005f, 0.007f), v3(tip - 0.006f, Y, -0.022f), &mat_brass_aged);
}
