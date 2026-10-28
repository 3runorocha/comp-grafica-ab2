#include "fire.h"
#include "../materials.h"
#include "../shader.h"
#include <math.h>

#define LOG_LEN 0.42f

// sprite de chama
typedef struct { float x, y, z, w, h, seed; int mode; } FlameSprite;

static const FlameSprite sprites[] = {
    {  0.00f, 0.12f,  0.00f, 0.40f, 0.44f, 0.11f, 0 },   // chama principal
    { -0.12f, 0.11f,  0.02f, 0.26f, 0.30f, 0.57f, 0 },
    {  0.13f, 0.11f, -0.01f, 0.26f, 0.33f, 0.83f, 0 },
    { -0.04f, 0.14f,  0.05f, 0.18f, 0.21f, 0.29f, 0 },   // línguas menores
    {  0.07f, 0.14f,  0.05f, 0.17f, 0.19f, 0.71f, 0 },
    {  0.00f, 0.00f,  0.02f, 0.80f, 0.45f, 0.00f, 1 },   // brilho da base
};
#define SPRITES ((int)(sizeof sprites / sizeof sprites[0]))

// recursos gl das chamas
static GLuint flame_prog, flame_vao, flame_vbo;

// inicializa shader e quad das chamas
static void flames_gl_init(void)
{
    if (flame_prog)
        return;
    flame_prog = shader_load("shaders/fire.vert", "shaders/fire.frag");
    // quad da chama
    static const float corners[] = { -0.5f, 0, 0.5f, 0, 0.5f, 1, -0.5f, 0, 0.5f, 1, -0.5f, 1 };
    glGenVertexArrays(1, &flame_vao);
    glGenBuffers(1, &flame_vbo);
    glBindVertexArray(flame_vao);
    glBindBuffer(GL_ARRAY_BUFFER, flame_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof corners, corners, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// tora irregular no torno
static Node *build_log(Node *parent, float radius, vec3 pos, float yaw, float seed)
{
    float profile[2 * 14];
    int n = 0;
    float h = LOG_LEN * 0.5f;
    profile[n++] = 0.0f;           profile[n++] = -h;
    profile[n++] = radius * 0.8f;  profile[n++] = -h;
    for (int i = 0; i <= 9; i++) {
        float t = (float)i / 9.0f;
        float r = radius * (1.0f + 0.07f * sinf(t * 9.0f + seed) + 0.05f * sinf(t * 23.0f + 2.0f * seed));
        profile[n++] = r;
        profile[n++] = -h + 0.01f + t * (LOG_LEN - 0.02f);
    }
    profile[n++] = radius * 0.8f;  profile[n++] = h;
    profile[n++] = 0.0f;           profile[n++] = h;

    // tora deitada
    Node *log = node_lathe(parent, profile, n / 2, pos, &mat_log);
    log->rot.z = 90.0f;
    log->rot.y = yaw;
    return log;
}

// monta a lareira
void fire_create(Fire *f, vec3 pos, float yaw_deg)
{
    flames_gl_init();
    f->lit = 0;
    f->growth = 0.0f;
    f->time = 0.0f;
    f->flicker = 0.0f;
    Node *root = f->root = node_group(NULL, pos);
    root->rot.y = yaw_deg;

    // grelha de ferro
    for (int side = -1; side <= 1; side += 2) {
        node_cylinder_between(root, v3(-0.22f, 0.08f, side * 0.08f), v3(0.22f, 0.08f, side * 0.08f), 0.01f, &mat_iron);
        for (int end = -1; end <= 1; end += 2)
            node_box(root, v3(0.02f, 0.08f, 0.02f), v3(end * 0.2f, 0.04f, side * 0.08f), &mat_iron);
    }
    node_sphere(root, 0.02f, v3(-0.2f, 0.1f, 0.08f), &mat_iron);
    node_sphere(root, 0.02f, v3(0.2f, 0.1f, 0.08f), &mat_iron);

    // carvões sob a grelha
    unsigned seed = 12345u;
    for (int i = 0; i < 26; i++) {
        float r[6];
        for (int k = 0; k < 6; k++) {
            seed = seed * 1103515245u + 12345u;
            r[k] = (float)((seed >> 16) & 0x7fff) / 32767.0f;
        }
        float s = 0.022f + 0.022f * r[0];
        Node *coal = node_box(root, v3(s, s * (0.45f + 0.3f * r[5]), s * (0.6f + 0.4f * r[4])),
                              v3(-0.17f + 0.34f * r[1], 0.008f, -0.09f + 0.18f * r[2]), &mat_coal);
        coal->rot = v3(30.0f * (r[3] - 0.5f), 360.0f * r[4], 30.0f * (r[5] - 0.5f));
    }

    // lenha
    build_log(root, 0.05f, v3(0, 0.13f, -0.055f), 4.0f, 0.3f);
    build_log(root, 0.045f, v3(0, 0.125f, 0.055f), -6.0f, 1.9f);
    build_log(root, 0.042f, v3(0.02f, 0.205f, 0), 28.0f, 3.7f);

    fire_update(f, 0.0f);
}

// acende/apaga o fogo
void fire_toggle(Fire *f)
{
    f->lit = !f->lit;
}

// crescimento suavizado
static float smooth_growth(const Fire *f)
{
    return f->growth * f->growth * (3.0f - 2.0f * f->growth);
}

// animação do fogo
void fire_update(Fire *f, float dt)
{
    f->time += dt;
    float t = f->time;

    // crescimento do fogo
    if (f->lit)
        f->growth = fminf(1.0f, f->growth + 0.4f * dt);
    else
        f->growth = fmaxf(0.0f, f->growth - 1.0f * dt);
    float g = smooth_growth(f);

    // tremulação do fogo
    f->flicker = 0.8f + 0.1f * sinf(t * 9.0f) + 0.07f * sinf(t * 15.3f + 0.7f) + 0.03f * sinf(t * 31.0f);

    // brasas da lenha e carvão
    mat_log.emissive = v3_scale(v3(1.3f, 1.0f, 1.0f), g * f->flicker);
    mat_coal.emissive = v3_scale(v3(3.2f, 0.9f, 0.2f), (0.3f + 0.7f * g) * f->flicker * (f->growth > 0.0f));
}

// luz pontual do fogo
int fire_light(const Fire *f, PointLight *out)
{
    if (f->growth <= 0.0f)
        return 0;
    float g = smooth_growth(f);
    out->pos = v3_add(f->root->pos, v3(0, 0.3f, 0));
    out->color = v3_scale(v3(1.0f, 0.45f, 0.15f), 2.4f * g * f->flicker);
    out->range = 4.5f;
    out->dir = v3(0, -1, 0);
    out->spot_cos = -2.0f;   // omnidirecional
    return 1;
}

// local para mundo
static vec3 local_to_world(const Fire *f, vec3 p)
{
    float a = deg2rad(f->root->rot.y);
    return v3_add(f->root->pos, v3(p.x * cosf(a) + p.z * sinf(a), p.y, -p.x * sinf(a) + p.z * cosf(a)));
}

// chamas em billboard
void fire_draw_flames(const Fire *f, const mat4 *view, const mat4 *proj)
{
    float g = smooth_growth(f);
    if (g <= 0.0f)
        return;

    mat4 vp = m4_mul(*proj, *view);
    // vetor direita da câmera
    vec3 right = v3(view->m[0], view->m[4], view->m[8]);

    glUseProgram(flame_prog);
    shader_set_mat4(flame_prog, "u_vp", &vp);
    shader_set_vec3(flame_prog, "u_cam_right", right);
    shader_set_float(flame_prog, "u_time", f->time);

    // mistura aditiva sem profundidade
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glBindVertexArray(flame_vao);

    // desenha cada chama
    for (int i = 0; i < SPRITES; i++) {
        const FlameSprite *s = &sprites[i];
        // escala pelo crescimento do fogo
        float k = (float)i * 1.37f, t = f->time;
        float stretch = 1.0f + 0.12f * sinf(t * 6.0f + k);
        shader_set_vec3(flame_prog, "u_center", local_to_world(f, v3(s->x, s->y * g, s->z)));
        shader_set_float(flame_prog, "u_seed", s->seed);
        shader_set_int(flame_prog, "u_mode", s->mode);
        shader_set_float(flame_prog, "u_intensity", g * f->flicker * 1.15f);
        glUniform2f(glGetUniformLocation(flame_prog, "u_size"), s->w * g, s->h * g * (s->mode ? 1.0f : stretch));
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // restaura estado gl
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

// ponto de interação
vec3 fire_spot(const Fire *f)
{
    return local_to_world(f, v3(0, 0, 0.7f));
}
