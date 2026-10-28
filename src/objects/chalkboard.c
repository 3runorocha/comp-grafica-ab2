#include "chalkboard.h"
#include "../materials.h"
#include <math.h>

// geometria das pernas da frente
#define LEG_BOTTOM_X 0.32f
#define LEG_TOP_X    0.10f
#define LEG_BOTTOM_Z 0.10f
#define LEG_TOP_Z   -0.06f
#define LEG_TOP_Y    1.70f
#define LEG_T        0.04f    // seção das pernas

#define TRAY_Y   0.78f
#define BOARD_W  0.86f        // largura da lousa
#define BOARD_H  0.62f
#define FRAME    0.04f

// ponto da perna da frente
static vec3 front_leg_at(int side, float y)
{
    float t = y / LEG_TOP_Y;
    return v3(side * (LEG_BOTTOM_X + (LEG_TOP_X - LEG_BOTTOM_X) * t), y,
              LEG_BOTTOM_Z + (LEG_TOP_Z - LEG_BOTTOM_Z) * t);
}

// bandeja com beirada
static void build_tray(Node *root)
{
    Node *tray = node_box(root, v3(0.95f, 0.025f, 0.07f), v3(0, TRAY_Y, 0.06f), &mat_wood_easel);
    node_box(tray, v3(0.95f, 0.03f, 0.012f), v3(0, 0.02f, 0.035f), &mat_wood_easel);

    // giz e apagador
    Node *c1 = node_cylinder(tray, 0.006f, 0.07f, v3(-0.25f, 0.019f, 0.005f), &mat_chalk);
    c1->rot.z = 90.0f;
    Node *c2 = node_cylinder(tray, 0.006f, 0.045f, v3(-0.15f, 0.019f, 0.0f), &mat_chalk);
    c2->rot.z = 90.0f;
    c2->rot.y = 25.0f;
    Node *eraser = node_box(tray, v3(0.12f, 0.025f, 0.045f), v3(0.22f, 0.035f, 0), &mat_wood_varnish);
    node_box(eraser, v3(0.12f, 0.01f, 0.045f), v3(0, -0.0175f, 0), &mat_felt);
    eraser->rot.y = -8.0f;
}

// lousa inclinada com moldura
static void build_board(Node *root)
{
    float tilt = atan2f(LEG_BOTTOM_Z - LEG_TOP_Z, LEG_TOP_Y) * 180.0f / PI_F;
    float total_h = BOARD_H + 2 * FRAME;
    Node *board = node_group(root, v3(0, TRAY_Y + 0.015f + total_h * 0.5f, 0.04f));
    board->rot.x = -tilt;

    node_box_fit(board, v3(BOARD_W, BOARD_H, 0.015f), v3(0, 0, 0), &mat_slate);
    float fw = BOARD_W + 2 * FRAME;
    node_box(board, v3(fw, FRAME, 0.03f), v3(0, (BOARD_H + FRAME) * 0.5f, 0), &mat_wood_easel);
    node_box(board, v3(fw, FRAME, 0.03f), v3(0, -(BOARD_H + FRAME) * 0.5f, 0), &mat_wood_easel);
    node_box(board, v3(FRAME, BOARD_H, 0.03f), v3((BOARD_W + FRAME) * 0.5f, 0, 0), &mat_wood_easel);
    node_box(board, v3(FRAME, BOARD_H, 0.03f), v3(-(BOARD_W + FRAME) * 0.5f, 0, 0), &mat_wood_easel);
}

// monta o quadro-negro
void chalkboard_create(Chalkboard *c, vec3 pos, float yaw_deg)
{
    Node *root = c->root = node_group(NULL, pos);
    root->rot.y = yaw_deg;

    // pernas do cavalete
    for (int side = -1; side <= 1; side += 2)
        node_box_between(root, front_leg_at(side, 0), front_leg_at(side, LEG_TOP_Y), LEG_T, &mat_wood_easel);
    node_box_between(root, v3(0, LEG_TOP_Y - 0.08f, -0.10f), v3(0, 0, -0.55f), LEG_T, &mat_wood_easel);

    // travessa e pino do topo
    node_box_between(root, front_leg_at(-1, 0.35f), front_leg_at(1, 0.35f), 0.03f, &mat_wood_easel);
    node_cylinder_between(root, front_leg_at(-1, LEG_TOP_Y - 0.08f), front_leg_at(1, LEG_TOP_Y - 0.08f),
                          0.012f, &mat_steel);

    build_tray(root);
    build_board(root);
}
