#include "player.h"
#include "materials.h"
#include "renderer.h"
#include "room.h"
#include "sound.h"
#include <math.h>

#define MODEL_DIR    "assets/characters/wesker"
#define MODEL_SCALE  0.57f     // escala para metros
#define MODEL_FACING -90.0f    // gira a frente para +z

#define WALK_SPEED  1.4f       // m/s
#define BACK_SPEED  0.8f       // velocidade de ré
#define TURN_SPEED  150.0f     // graus/s
#define STEP_RATE   7.5f       // ritmo do passo
#define BODY_RADIUS 0.3f       // raio de colisão

// partes do esqueleto do re1
enum {
    TORSO = 0, HEAD = 1, HIPS = 2,
    THIGH_L = 3, SHIN_L = 4, FOOT_L = 5,
    THIGH_R = 6, SHIN_R = 7, FOOT_R = 8,
    ARM_L = 9, FOREARM_L = 10, HAND_L = 11,
    ARM_R = 12, FOREARM_R = 13, HAND_R = 14,
};

static Mesh fallback_box;
static float base_torso_y;   // altura original do tronco

// carrega o personagem
void player_init(Player *p, vec3 pos)
{
    p->pos = pos;
    p->yaw = 0.0f;
    p->walk_phase = 0.0f;
    p->walk_blend = 0.0f;
    p->has_model = character_load(&p->model, MODEL_DIR, MODEL_SCALE, MODEL_FACING) && p->model.part_count >= 15;
    if (p->has_model)
        base_torso_y = p->model.parts[TORSO]->pos.y;
    else
        fallback_box = mesh_cube();
}

// seno com amplitude em graus
static float deg_sin(float phase, float amplitude_deg)
{
    return amplitude_deg * sinf(phase);
}

// animação de andar
static void animate_walk(Player *p)
{
    Node **part = p->model.parts;
    float ph = p->walk_phase, b = p->walk_blend;

    // pernas
    float leg = deg_sin(ph, 28.0f) * b;
    part[THIGH_L]->rot.z = leg;
    part[THIGH_R]->rot.z = -leg;
    part[SHIN_L]->rot.z = -fmaxf(0.0f, sinf(ph + 1.3f)) * 40.0f * b;
    part[SHIN_R]->rot.z = -fmaxf(0.0f, sinf(ph + 1.3f + PI_F)) * 40.0f * b;
    part[FOOT_L]->rot.z = -0.5f * part[THIGH_L]->rot.z;
    part[FOOT_R]->rot.z = -0.5f * part[THIGH_R]->rot.z;

    // braços
    float arm = deg_sin(ph, 22.0f) * b;
    part[ARM_L]->rot.z = -arm;
    part[ARM_R]->rot.z = arm;
    part[FOREARM_L]->rot.z = 12.0f + 10.0f * b;
    part[FOREARM_R]->rot.z = 12.0f + 10.0f * b;

    // tronco e cabeça
    part[TORSO]->pos.y = base_torso_y + fabsf(sinf(ph)) * 0.035f * b / MODEL_SCALE;
    part[HEAD]->rot.z = -2.0f * b;
}

// movimento, colisão e passos
void player_update(Player *p, GLFWwindow *win, float dt)
{
    // giro
    int turning = 0;
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) { p->yaw += TURN_SPEED * dt; turning = 1; }
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) { p->yaw -= TURN_SPEED * dt; turning = 1; }

    // anda para frente/trás
    float speed = 0.0f;
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) speed += WALK_SPEED;
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) speed -= BACK_SPEED;

    float y = deg2rad(p->yaw);
    vec3 fwd = v3(sinf(y), 0.0f, cosf(y));
    p->pos = v3_add(p->pos, v3_scale(fwd, speed * dt));
    vec3 before = p->pos;
    room_collide(&p->pos, BODY_RADIUS);

    // passo de ré ao ser empurrado
    if (speed == 0.0f && dt > 0.0f) {
        vec3 push = v3_sub(p->pos, before);
        float pushed = v3_dot(push, fwd) / dt;
        if (fabsf(pushed) > 0.05f)
            speed = pushed;
    }

    // passinhos ao girar parado
    float anim_speed = speed;
    float target = speed != 0.0f ? 1.0f : 0.0f;
    if (speed == 0.0f && turning) {
        anim_speed = 0.7f * WALK_SPEED;
        target = 0.45f;
    }

    // avança o ciclo e toca os passos
    float before_step = floorf((p->walk_phase - PI_F * 0.5f) / PI_F);
    p->walk_phase += STEP_RATE * (anim_speed / WALK_SPEED) * dt;
    float after_step = floorf((p->walk_phase - PI_F * 0.5f) / PI_F);
    if (after_step != before_step && p->walk_blend > 0.35f)
        sound_play(((long)after_step & 1) ? SND_STEP_R : SND_STEP_L);
    p->walk_blend += (target - p->walk_blend) * fminf(1.0f, 8.0f * dt);

    // anima as articulações
    if (p->has_model)
        animate_walk(p);
}

// desenha o personagem
void player_draw(Player *p)
{
    if (p->has_model) {
        // posiciona o modelo
        p->model.root->pos = p->pos;
        p->model.root->rot.y = p->yaw;
        node_draw(p->model.root, NULL);
        return;
    }
    // caixa provisória sem modelo
    mat4 model = m4_mul(m4_translate(v3(p->pos.x, 0.85f, p->pos.z)),
                 m4_mul(m4_rotate(deg2rad(p->yaw), v3(0, 1, 0)),
                        m4_scale(v3(0.45f, 1.7f, 0.25f))));
    renderer_draw(&fallback_box, &model, &mat_cloth);
}
