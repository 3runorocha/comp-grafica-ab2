#include "chair.h"
#include "../materials.h"
#include <math.h>

// medidas da cadeira
#define SEAT_W 0.44f
#define SEAT_D 0.42f
#define SEAT_Y 0.46f     // altura do assento
#define LEG    0.04f
#define BACK_H 0.50f     // altura do encosto
#define BACK_TILT -8.0f  // inclinação do encosto

// encosto inclinado
static void build_backrest(Node *root, float lx, float lz)
{
    Node *back = node_group(root, v3(0, SEAT_Y - 0.03f, -lz));
    back->rot.x = BACK_TILT;

    // montantes com remate
    for (int side = -1; side <= 1; side += 2) {
        node_box(back, v3(LEG, BACK_H, LEG), v3(side * lx, BACK_H * 0.5f, 0), &mat_wood_chair);
        node_cylinder(back, 0.024f, 0.03f, v3(side * lx, BACK_H + 0.015f, 0), &mat_wood_chair);
    }
    // travessas e ripas do encosto
    node_box(back, v3(2 * lx + LEG, 0.08f, 0.03f), v3(0, BACK_H - 0.05f, 0), &mat_wood_chair);
    node_box(back, v3(2 * lx, 0.03f, 0.025f), v3(0, 0.12f, 0), &mat_wood_chair);
    float slat_bottom = 0.135f, slat_top = BACK_H - 0.09f;
    for (int i = -1; i <= 1; i++)
        node_box(back, v3(0.045f, slat_top - slat_bottom, 0.015f),
                 v3(i * 0.1f, (slat_top + slat_bottom) * 0.5f, 0), &mat_wood_chair);
}

// monta a cadeira
void chair_create(Chair *c, vec3 pos, float yaw_deg)
{
    Node *root = c->root = node_group(NULL, pos);
    c->tucked = c->pulled = pos;
    c->out = c->target = 0.0f;
    root->rot.y = yaw_deg;

    float lx = SEAT_W * 0.5f - LEG * 0.5f - 0.01f;
    float lz = SEAT_D * 0.5f - LEG * 0.5f - 0.01f;
    float leg_h = SEAT_Y - 0.03f;

    // pernas da cadeira
    for (int i = 0; i < 4; i++) {
        float x = (i & 1) ? lx : -lx, z = (i & 2) ? lz : -lz;
        node_box(root, v3(LEG, leg_h, LEG), v3(x, leg_h * 0.5f, z), &mat_wood_chair);
    }

    // moldura sob o assento
    float apron_y = SEAT_Y - 0.06f;
    node_box(root, v3(2 * lx, 0.06f, 0.02f), v3(0, apron_y, lz), &mat_wood_chair);
    node_box(root, v3(2 * lx, 0.06f, 0.02f), v3(0, apron_y, -lz), &mat_wood_chair);
    node_box(root, v3(0.02f, 0.06f, 2 * lz), v3(lx, apron_y, 0), &mat_wood_chair);
    node_box(root, v3(0.02f, 0.06f, 2 * lz), v3(-lx, apron_y, 0), &mat_wood_chair);

    // travas entre as pernas
    node_box(root, v3(0.022f, 0.022f, 2 * lz), v3(lx, 0.13f, 0), &mat_wood_chair);
    node_box(root, v3(0.022f, 0.022f, 2 * lz), v3(-lx, 0.13f, 0), &mat_wood_chair);
    node_box(root, v3(2 * lx, 0.022f, 0.022f), v3(0, 0.18f, lz), &mat_wood_chair);

    // assento com almofada de couro
    node_box(root, v3(SEAT_W, 0.03f, SEAT_D), v3(0, SEAT_Y - 0.015f, 0), &mat_wood_chair);
    node_box(root, v3(SEAT_W - 0.05f, 0.04f, SEAT_D - 0.06f), v3(0, SEAT_Y + 0.02f, 0.01f), &mat_leather);
    node_box(root, v3(SEAT_W - 0.09f, 0.015f, SEAT_D - 0.10f), v3(0, SEAT_Y + 0.045f, 0.01f), &mat_leather);

    build_backrest(root, lx, lz);
}

// define o trilho da cadeira
void chair_set_track(Chair *c, vec3 tucked, vec3 pulled)
{
    c->tucked = tucked;
    c->pulled = pulled;
    c->out = c->target = 0.0f;
    c->root->pos = tucked;
}

// alterna guardada/puxada
void chair_toggle(Chair *c)
{
    c->target = c->target > 0.5f ? 0.0f : 1.0f;
}

// deslizamento suave da cadeira
void chair_update(Chair *c, float dt)
{
    const float speed = 1.4f;
    if (c->out < c->target)
        c->out = fminf(c->target, c->out + speed * dt);
    else if (c->out > c->target)
        c->out = fmaxf(c->target, c->out - speed * dt);
    float t = c->out * c->out * (3.0f - 2.0f * c->out);
    c->root->pos = v3_add(c->tucked, v3_scale(v3_sub(c->pulled, c->tucked), t));
}
