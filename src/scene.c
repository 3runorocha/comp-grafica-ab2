#include "scene.h"
#include "renderer.h"
#include "objects/chair.h"
#include "objects/chalkboard.h"
#include "objects/crate.h"
#include "objects/desk.h"
#include "objects/fire.h"
#include "objects/globe.h"
#include "objects/key.h"
#include "objects/lamp.h"
#include "objects/stepladder.h"
#include "room.h"
#include "shadow.h"
#include "sound.h"
#include <math.h>
#include <stdio.h>

#define INTERACT_RANGE 0.8f   // alcance da tecla e
#define GLOBE_SCALE 1.6f      // escala do globo

static Desk desk;
static Crate crate;
static Stepladder ladder;
static Chair chair;
static Chalkboard board;
static Globe globe;
static Lamp lamp;
static Fire fire;
static Key key;

static int chair_obstacle;   // obstáculos móveis
static int drawer_obstacle;

// meia-largura no mundo
static void half_extents(float yaw_deg, float sx, float sz, float *hx, float *hz)
{
    int sideways = ((int)(yaw_deg < 0 ? -yaw_deg : yaw_deg) % 180) == 90;
    *hx = (sideways ? sz : sx) * 0.5f;
    *hz = (sideways ? sx : sz) * 0.5f;
}

// registra pegada como obstáculo
static int add_footprint(const char *name, vec3 center, float yaw_deg, float sx, float sz, float h)
{
    float hx, hz;
    half_extents(yaw_deg, sx, sz, &hx, &hz);
    return room_add_obstacle(name, center.x - hx, center.x + hx, center.z - hz, center.z + hz, h);
}

// ponto local da mesa para o mundo
static vec3 desk_local(vec3 p)
{
    float a = deg2rad(desk.root->rot.y);
    return v3_add(desk.root->pos, v3(p.x * cosf(a) + p.z * sinf(a), p.y, -p.x * sinf(a) + p.z * cosf(a)));
}

// cria e posiciona os objetos
void scene_init(void)
{
    // escrivaninha, abajur e cadeira
    const float desk_yaw = 90.0f;
    desk_create(&desk, v3(-2.91f + DESK_D * 0.5f + 0.02f, 0, 2.96f), desk_yaw);
    lamp_create(&lamp, desk_local(v3(-0.45f, 0.76f, -0.05f)), desk_yaw);
    chair_create(&chair, desk_local(v3(-0.45f, 0, 0.14f)), desk_yaw + 180.0f);
    // trilho da cadeira
    chair_set_track(&chair, desk_local(v3(-0.45f, 0, 0.14f)), desk_local(v3(-0.45f, 0, 0.59f)));

    // chave dentro da gaveta
    key_create(&key, v3(-0.06f, -0.035f, 0.12f), 0.0f);
    node_attach(desk.drawer, key.root);

    // quadro-negro na parede do fundo
    chalkboard_create(&board, v3(-0.28f, 0, -4.92f), 0.0f);

    // caixote junto à janela
    crate_create(&crate, v3(-2.58f, 0, -1.37f), 0.0f);

    // escadinha entre porta e lareira
    stepladder_create(&ladder, v3(2.0f, 0, -2.2f), -90.0f);

    // globo no canto esquerdo
    globe_create(&globe, v3(-2.55f, 0, -5.14f), 0.0f);
    globe.root->scale = v3(GLOBE_SCALE, GLOBE_SCALE, GLOBE_SCALE);

    // fogo dentro da lareira
    fire_create(&fire, v3(2.05f, 0.05f, 0.48f), -90.0f);

    // obstáculos dos objetos
    add_footprint("escrivaninha", desk.root->pos, desk_yaw, DESK_W, DESK_D, 0.76f);
    chair_obstacle = add_footprint("cadeira", chair.root->pos, desk_yaw, CHAIR_W, CHAIR_D, 0.95f);
    drawer_obstacle = room_add_obstacle("gaveta", 0, 0, 0, 0, 0.7f);
    add_footprint("quadro-negro", v3_add(board.root->pos, v3(0, 0, -0.22f)), 0.0f, 0.95f, 0.66f, 1.70f);
    add_footprint("caixote", crate.root->pos, 0.0f, CRATE_SIZE, CRATE_SIZE, CRATE_SIZE);
    add_footprint("escadinha", v3_add(ladder.root->pos, v3(0.02f, 0, 0)), -90.0f, 0.46f, 0.58f, 0.56f);
    add_footprint("globo", globe.root->pos, 0.0f, 0.36f * GLOBE_SCALE, 0.36f * GLOBE_SCALE, 1.0f * GLOBE_SCALE);
}

// anima os objetos
void scene_update(float dt)
{
    desk_update(&desk, dt);
    chair_update(&chair, dt);
    // colisão acompanha a cadeira
    float hx, hz;
    half_extents(chair.root->rot.y, CHAIR_W, CHAIR_D, &hx, &hz);
    vec3 c = chair.root->pos;
    room_move_obstacle(chair_obstacle, c.x - hx, c.x + hx, c.z - hz, c.z + hz);

    // colisão acompanha a gaveta
    float front = DESK_D * 0.5f, out = front + desk.drawer->pos.z;
    vec3 a = desk_local(v3(DRAWER_X - DRAWER_W * 0.5f, 0, front));
    vec3 b = desk_local(v3(DRAWER_X + DRAWER_W * 0.5f, 0, out));
    room_move_obstacle(drawer_obstacle, fminf(a.x, b.x), fmaxf(a.x, b.x), fminf(a.z, b.z), fmaxf(a.z, b.z));
    globe_update(&globe, dt);
    lamp_update(&lamp, dt);
    fire_update(&fire, dt);
}

// luzes e desenho dos objetos
void scene_draw(void)
{
    PointLight lights[MAX_POINT_LIGHTS];
    int n = 0;
    n += lamp_light(&lamp, &lights[n]);
    n += fire_light(&fire, &lights[n]);
    // spot da arandela
    lights[n++] = (PointLight){ .pos = v3(-0.25f, 2.30f, -5.40f), .color = v3(1.43f, 0.95f, 0.49f),
                                .range = 4.0f, .dir = v3(0, -1.0f, 0.3f), .spot_cos = 0.55f };
    renderer_set_point_lights(lights, n);

    // desenha os objetos
    node_draw(desk.root, NULL);
    node_draw(crate.root, NULL);
    node_draw(ladder.root, NULL);
    node_draw(chair.root, NULL);
    node_draw(board.root, NULL);
    node_draw(globe.root, NULL);
    node_draw(lamp.root, NULL);
    node_draw(fire.root, NULL);
}

// sombra do tamanho da pegada
static void footprint_shadow(vec3 center, float yaw_deg, float sx, float sz, float strength)
{
    float hx, hz;
    half_extents(yaw_deg, sx, sz, &hx, &hz);
    shadow_draw(center, hx * 1.35f + 0.08f, hz * 1.35f + 0.08f, strength);
}

// sombras dos objetos e do personagem
void scene_draw_shadows(const mat4 *view, const mat4 *proj, vec3 player_pos)
{
    shadow_begin(view, proj);
    footprint_shadow(desk.root->pos, desk.root->rot.y, DESK_W, DESK_D, 0.65f);
    footprint_shadow(chair.root->pos, chair.root->rot.y, CHAIR_W, CHAIR_D, 0.60f);
    footprint_shadow(v3_add(board.root->pos, v3(0, 0, -0.22f)), 0.0f, 0.80f, 0.55f, 0.55f);
    footprint_shadow(crate.root->pos, 0.0f, CRATE_SIZE, CRATE_SIZE, 0.70f);
    footprint_shadow(ladder.root->pos, -90.0f, 0.46f, 0.58f, 0.50f);
    footprint_shadow(globe.root->pos, 0.0f, 0.32f * GLOBE_SCALE, 0.32f * GLOBE_SCALE, 0.60f);
    shadow_draw(player_pos, 0.42f, 0.42f, 0.75f);   // personagem
    shadow_end();
}

// chamas da lareira
void scene_draw_behind(const mat4 *view, const mat4 *proj)
{
    fire_draw_flames(&fire, view, proj);
}

// pontos de interação do abajur e da cadeira
static vec3 lamp_spot(void)  { return desk_local(v3(-0.80f, 0, 0.60f)); }
static vec3 chair_spot(void) { return desk_local(v3(-0.30f, 0, 0.95f)); }

// distância no chão
static float dist_xz(vec3 a, vec3 b)
{
    return v3_length(v3(a.x - b.x, 0, a.z - b.z));
}

// abre/fecha a gaveta
static void act_drawer(void)
{
    desk_toggle_drawer(&desk);
    sound_play(SND_DRAWER);
    printf("Gaveta: %s\n", desk.target > 0.5f ? "abrindo" : "fechando");
}

// gira o globo
static void act_globe(void)
{
    globe_push(&globe);
    sound_play(SND_GLOBE);
    printf("Globo girando\n");
}

// acende/apaga a lareira
static void act_fire(void)
{
    fire_toggle(&fire);
    sound_loop(SND_FIRE, fire.lit);   // loop som da lareira
    puts(fire.lit ? "Lareira acesa" : "Lareira apagada");
}

// liga/desliga o abajur
static void act_lamp(void)
{
    lamp_toggle(&lamp);
    sound_play(SND_SWITCH);
    printf("Abajur %s\n", lamp.on ? "ligado" : "desligado");
}

// puxa/guarda a cadeira
static void act_chair(void)
{
    chair_toggle(&chair);
    sound_play(SND_CHAIR);
    printf("Cadeira: %s\n", chair.target > 0.5f ? "puxando" : "guardando");
}

// interage com o objeto mais próximo
void scene_interact(vec3 pos)
{
    // pontos de interação e ações
    struct { vec3 at; void (*act)(void); } spots[] = {
        { desk_handle_pos(&desk), act_drawer },
        { globe.root->pos, act_globe },
        { lamp_spot(), act_lamp },
        { chair_spot(), act_chair },
        { fire_spot(&fire), act_fire },
    };

    // escolhe o mais próximo no alcance
    int best = -1;
    float best_d = INTERACT_RANGE;
    for (int i = 0; i < (int)(sizeof spots / sizeof spots[0]); i++) {
        float d = dist_xz(pos, spots[i].at);
        if (d < best_d) {
            best_d = d;
            best = i;
        }
    }
    if (best >= 0)
        spots[best].act();
}
