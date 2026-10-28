#include "globe.h"
#include "../materials.h"
#include <math.h>

#define GLOBE_R   0.16f
#define RING_R    0.175f
#define CENTER_Y  0.83f
#define AXIS_TILT 23.5f   // inclinação do eixo

// perfil do pedestal
static const float pedestal[] = {
    0.000f, 0.000f,   0.160f, 0.000f,   0.160f, 0.025f,   0.120f, 0.040f,
    0.060f, 0.060f,   0.035f, 0.090f,   0.030f, 0.300f,   0.048f, 0.330f,
    0.048f, 0.350f,   0.030f, 0.380f,   0.027f, 0.560f,   0.045f, 0.590f,
    0.040f, 0.615f,   0.000f, 0.620f,
};

// monta o globo
void globe_create(Globe *g, vec3 pos, float yaw_deg)
{
    g->speed = 0.0f;
    Node *root = g->root = node_group(NULL, pos);
    root->rot.y = yaw_deg;

    // pedestal torneado
    node_lathe(root, pedestal, (int)(sizeof pedestal / sizeof pedestal[0] / 2), v3(0, 0, 0), &mat_wood_varnish);

    // grupo inclinado
    Node *tilt = node_group(root, v3(0, CENTER_Y, 0));
    tilt->rot.z = AXIS_TILT;

    // anel meridiano e pinos
    Node *ring = node_torus(tilt, RING_R, 0.006f, v3(0, 0, 0), &mat_brass);
    ring->rot.x = 90.0f;
    node_cylinder(tilt, 0.006f, 0.03f, v3(0, GLOBE_R + 0.008f, 0), &mat_brass);
    node_cylinder(tilt, 0.006f, 0.03f, v3(0, -GLOBE_R - 0.008f, 0), &mat_brass);

    // esfera giratória
    g->spin = node_group(tilt, v3(0, 0, 0));
    node_sphere(g->spin, GLOBE_R, v3(0, 0, 0), &mat_globe_map);

    // haste do pedestal ao anel
    float a = deg2rad(AXIS_TILT);
    vec3 ring_bottom = v3(RING_R * sinf(a), CENTER_Y - RING_R * cosf(a), 0);
    node_cylinder_between(root, v3(0, 0.6f, 0), ring_bottom, 0.008f, &mat_brass);
}

// impulso de giro
void globe_push(Globe *g)
{
    g->speed += 240.0f;
    if (g->speed > 720.0f)
        g->speed = 720.0f;
}

// giro com atrito
void globe_update(Globe *g, float dt)
{
    g->spin->rot.y = fmodf(g->spin->rot.y + g->speed * dt, 360.0f);
    g->speed *= expf(-0.9f * dt);
    if (g->speed < 1.0f)
        g->speed = 0.0f;
}
