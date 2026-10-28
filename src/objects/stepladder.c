#include "stepladder.h"
#include "../materials.h"

// medidas da escadinha
#define RAIL_X   0.21f    // tubos da frente
#define LEG_X    0.185f   // pernas de trás
#define TOP_Y    0.56f
#define RAIL_R   0.016f
#define LEG_R    0.014f

// z do tubo da frente
static float rail_z(float y) { return 0.26f - 0.28f * (y / TOP_Y); }
// z da perna de trás
static float leg_z(float y) { return -0.30f + 0.24f * (y / (TOP_Y - 0.02f)); }

// degrau com frisos e suportes
static void build_tread(Node *root, float y, float z, vec3 size)
{
    Node *t = node_box(root, size, v3(0, y, z), &mat_metal_painted);
    const int ridges = 5;
    for (int i = 0; i < ridges; i++) {
        float rz = -size.z * 0.5f + size.z * (i + 0.5f) / ridges;
        node_box(t, v3(size.x - 0.03f, 0.006f, 0.012f), v3(0, size.y * 0.5f + 0.003f, rz), &mat_steel);
    }
    for (int side = -1; side <= 1; side += 2)
        node_box(t, v3(0.008f, 0.05f, size.z * 0.8f), v3(side * (size.x * 0.5f + 0.004f), -0.02f, 0),
                 &mat_metal_painted);
}

// monta a escadinha
void stepladder_create(Stepladder *s, vec3 pos, float yaw_deg)
{
    Node *root = s->root = node_group(NULL, pos);
    root->rot.y = yaw_deg;

    for (int side = -1; side <= 1; side += 2) {
        float rx = side * RAIL_X, lx = side * LEG_X;

        // tubo da frente e perna de trás
        node_cylinder_between(root, v3(rx, 0.0f, rail_z(0)), v3(rx, TOP_Y, rail_z(TOP_Y)), RAIL_R,
                              &mat_metal_painted);
        node_cylinder_between(root, v3(lx, 0.0f, leg_z(0)), v3(lx, TOP_Y - 0.02f, leg_z(TOP_Y - 0.02f)),
                              LEG_R, &mat_metal_painted);

        // barra lateral de trava
        float by = 0.18f, z0 = rail_z(by), z1 = leg_z(by);
        node_box(root, v3(0.008f, 0.025f, z0 - z1), v3(side * 0.198f, by, (z0 + z1) * 0.5f),
                 &mat_metal_painted);

        // pés de borracha
        node_cylinder(root, 0.024f, 0.03f, v3(rx, 0.015f, rail_z(0)), &mat_rubber);
        node_cylinder(root, 0.022f, 0.03f, v3(lx, 0.015f, leg_z(0)), &mat_rubber);
    }

    // travessa das pernas de trás
    float cy = 0.15f;
    node_cylinder_between(root, v3(-LEG_X, cy, leg_z(cy)), v3(LEG_X, cy, leg_z(cy)), 0.011f, &mat_metal_painted);

    // degrau e plataforma do topo
    build_tread(root, 0.27f, rail_z(0.27f) - 0.025f, v3(0.40f, 0.025f, 0.15f));
    build_tread(root, TOP_Y - 0.005f, -0.07f, v3(0.44f, 0.03f, 0.26f));
}
