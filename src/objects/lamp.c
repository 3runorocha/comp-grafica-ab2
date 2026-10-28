#include "lamp.h"
#include "../materials.h"

#define BULB_Y  0.385f
#define SHADE_Y 0.30f     // base da cúpula

// perfil da base torneada
static const float base_profile[] = {
    0.000f, 0.000f,   0.075f, 0.000f,   0.075f, 0.012f,   0.060f, 0.020f,
    0.050f, 0.032f,   0.064f, 0.070f,   0.070f, 0.115f,   0.058f, 0.165f,
    0.032f, 0.198f,   0.020f, 0.214f,   0.020f, 0.230f,   0.000f, 0.232f,
};
// perfil da cúpula com espessura
static const float shade_profile[] = {
    0.150f, 0.000f,   0.085f, 0.160f,   0.081f, 0.160f,   0.146f, 0.000f,   0.150f, 0.000f,
};
// perfil do soquete
static const float socket_profile[] = {
    0.000f, 0.000f,   0.016f, 0.000f,   0.018f, 0.010f,   0.018f, 0.035f,
    0.014f, 0.042f,   0.000f, 0.042f,
};

#define COUNT(a) ((int)(sizeof(a) / sizeof((a)[0]) / 2))

// monta o abajur
void lamp_create(Lamp *l, vec3 pos, float yaw_deg)
{
    l->on = 0;
    Node *root = l->root = node_group(NULL, pos);
    root->rot.y = yaw_deg;

    // base, haste, soquete e lâmpada
    node_lathe(root, base_profile, COUNT(base_profile), v3(0, 0, 0), &mat_ceramic);
    node_cylinder(root, 0.008f, 0.11f, v3(0, 0.285f, 0), &mat_brass);
    node_lathe(root, socket_profile, COUNT(socket_profile), v3(0, 0.335f, 0), &mat_brass);
    node_sphere(root, 0.028f, v3(0, BULB_Y, 0), &mat_bulb);

    // armação da cúpula
    for (int side = -1; side <= 1; side += 2)
        node_cylinder_between(root, v3(0, 0.36f, 0), v3(side * 0.082f, SHADE_Y + 0.158f, 0), 0.0025f, &mat_brass);

    // cúpula
    node_lathe(root, shade_profile, COUNT(shade_profile), v3(0, SHADE_Y, 0), &mat_lampshade);

    // corrente de puxar
    for (int i = 0; i < 7; i++)
        node_sphere(root, 0.0028f, v3(0.02f, 0.345f - i * 0.008f, 0), &mat_brass);
    node_cylinder(root, 0.005f, 0.014f, v3(0.02f, 0.345f - 7 * 0.008f - 0.004f, 0), &mat_brass);

    lamp_update(l, 0.0f);
}

// liga/desliga o abajur
void lamp_toggle(Lamp *l)
{
    l->on = !l->on;
}

// brilho da lâmpada e cúpula
void lamp_update(Lamp *l, float dt)
{
    (void)dt;
    mat_bulb.emissive = l->on ? v3(1.0f, 0.92f, 0.75f) : v3(0.0f, 0.0f, 0.0f);
    mat_lampshade.emissive = l->on ? v3(0.55f, 0.42f, 0.22f) : v3(0.0f, 0.0f, 0.0f);
}

// luz pontual do abajur
int lamp_light(const Lamp *l, PointLight *out)
{
    if (!l->on)
        return 0;
    out->pos = v3_add(l->root->pos, v3(0, BULB_Y, 0));
    out->color = v3(1.80f, 1.19f, 0.61f);   // 2700 K incandescente
    out->range = 3.2f;
    out->dir = v3(0, -1, 0);
    out->spot_cos = -2.0f;   // omnidirecional
    return 1;
}
