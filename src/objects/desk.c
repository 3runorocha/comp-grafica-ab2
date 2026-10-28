#include "desk.h"
#include "../materials.h"
#include <math.h>

// medidas da mesa
#define DESK_H 0.76f        // altura do tampo
#define TOP_T  0.035f       // espessura do tampo
#define LEG    0.05f        // seção da perna
#define APRON_H 0.10f       // altura da saia
#define DRAWER_TRAVEL 0.32f // curso da gaveta

// gaveta com puxador
static void build_drawer(Desk *d, Node *parent, float apron_y, float front_z)
{
    Node *g = d->drawer = node_group(parent, v3(DRAWER_X, apron_y, 0));
    float inner_w = DRAWER_W - 0.04f;
    float len = DESK_D - 0.12f;
    float zc = front_z - 0.01f - len * 0.5f;

    node_box(g, v3(DRAWER_W, APRON_H - 0.012f, 0.02f), v3(0, 0, front_z), &mat_wood_varnish);
    node_box(g, v3(inner_w, 0.01f, len), v3(0, -0.04f, zc), &mat_wood_inner);
    node_box(g, v3(0.012f, 0.07f, len), v3(-inner_w * 0.5f, -0.005f, zc), &mat_wood_inner);
    node_box(g, v3(0.012f, 0.07f, len), v3(inner_w * 0.5f, -0.005f, zc), &mat_wood_inner);
    node_box(g, v3(inner_w, 0.07f, 0.012f), v3(0, -0.005f, zc - len * 0.5f), &mat_wood_inner);

    // puxador
    Node *knob = node_cylinder(g, 0.014f, 0.03f, v3(0, 0, front_z + 0.025f), &mat_brass);
    knob->rot.x = 90.0f;
    node_cylinder(knob, 0.022f, 0.008f, v3(0, 0.017f, 0), &mat_brass);
}

// monta a escrivaninha
void desk_create(Desk *d, vec3 pos, float yaw_deg)
{
    d->open = d->target = 0.0f;
    Node *root = d->root = node_group(NULL, pos);
    root->rot.y = yaw_deg;

    float hx = DESK_W * 0.5f - LEG * 0.5f - 0.03f;
    float hz = DESK_D * 0.5f - LEG * 0.5f - 0.03f;
    float leg_h = DESK_H - TOP_T;
    float apron_y = DESK_H - TOP_T - APRON_H * 0.5f;
    float front_z = hz + LEG * 0.5f - 0.01f;

    // tampo
    node_box(root, v3(DESK_W, TOP_T, DESK_D), v3(0, DESK_H - TOP_T * 0.5f, 0), &mat_wood_varnish);

    // pernas com pé
    for (int i = 0; i < 4; i++) {
        float x = (i & 1) ? hx : -hx, z = (i & 2) ? hz : -hz;
        node_box(root, v3(LEG, leg_h, LEG), v3(x, leg_h * 0.5f, z), &mat_wood_varnish);
        node_box(root, v3(LEG + 0.016f, 0.04f, LEG + 0.016f), v3(x, 0.02f, z), &mat_wood_varnish);
    }

    // saia sob o tampo
    node_box(root, v3(2 * hx, APRON_H, 0.02f), v3(0, apron_y, -hz), &mat_wood_varnish);
    node_box(root, v3(0.02f, APRON_H, 2 * hz), v3(-hx, apron_y, 0), &mat_wood_varnish);
    node_box(root, v3(0.02f, APRON_H, 2 * hz), v3(hx, apron_y, 0), &mat_wood_varnish);

    float drawer_l = DRAWER_X - DRAWER_W * 0.5f, drawer_r = DRAWER_X + DRAWER_W * 0.5f;
    float left_w = drawer_l + hx;
    node_box(root, v3(left_w, APRON_H, 0.02f), v3(-hx + left_w * 0.5f, apron_y, front_z), &mat_wood_varnish);
    float right_w = hx - drawer_r;
    node_box(root, v3(right_w, APRON_H, 0.02f), v3(drawer_r + right_w * 0.5f, apron_y, front_z), &mat_wood_varnish);

    // nicho da gaveta
    node_box(root, v3(DRAWER_W, 0.01f, 2 * hz), v3(DRAWER_X, apron_y - APRON_H * 0.5f, 0), &mat_shadow);
    node_box(root, v3(0.01f, APRON_H, 2 * hz), v3(drawer_l - 0.005f, apron_y, 0), &mat_shadow);

    build_drawer(d, root, apron_y, front_z);
}

// abre/fecha a gaveta
void desk_toggle_drawer(Desk *d)
{
    d->target = d->target > 0.5f ? 0.0f : 1.0f;
}

// animação da gaveta
void desk_update(Desk *d, float dt)
{
    const float speed = 2.0f;
    if (d->open < d->target) {
        d->open += speed * dt;
        if (d->open > d->target) d->open = d->target;
    } else if (d->open > d->target) {
        d->open -= speed * dt;
        if (d->open < d->target) d->open = d->target;
    }
    // suavização smoothstep
    float t = d->open * d->open * (3.0f - 2.0f * d->open);
    d->drawer->pos.z = t * DRAWER_TRAVEL;
}

// posição da frente da gaveta
vec3 desk_handle_pos(const Desk *d)
{
    vec3 local = v3(DRAWER_X, 0, DESK_D * 0.5f + 0.15f);
    float a = deg2rad(d->root->rot.y);
    vec3 r = v3(local.x * cosf(a) + local.z * sinf(a), 0, -local.x * sinf(a) + local.z * cosf(a));
    return v3_add(d->root->pos, r);
}
