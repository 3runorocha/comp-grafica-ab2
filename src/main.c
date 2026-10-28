// programa principal da lesson room
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "background.h"
#include "calib.h"
#include "camera.h"
#include "mathlib.h"
#include "materials.h"
#include "player.h"
#include "renderer.h"
#include "scene.h"
#include "shadow.h"
#include "room.h"
#include "sound.h"
#include "texture.h"
#include "zones.h"

#define WINDOW_W 1024
#define WINDOW_H 768
#define ASPECT (4.0f / 3.0f)
#define DEFAULT_CAMERAS_FILE "assets/cameras.txt"
#define ZONES_FILE "assets/zones.txt"
#define ROOM_FILE "assets/room.txt"
#define LIGHTS_FILE "assets/lights.txt"

// estado global do app
typedef struct {
    GLFWwindow *win;
    CameraShot shots[MAX_SHOTS];
    int shot_count;
    int cur;
    int calib;
    int fullscreen;
    int win_x, win_y, win_w, win_h;   // janela antes da tela cheia
    const char *screenshot_path;      // arquivo da captura
    const char *cameras_file;
    Player player;
} App;

static App app;

// imprime os controles
static void print_controls(void)
{
    printf("\n=== Controles ===\n"
           "  W/S        andar para frente/tras (controle tanque)\n"
           "  A/D        girar\n"
           "  E          interagir com o objeto mais proximo\n"
           "  1-%d / Tab  trocar angulo de camera\n"
           "  F1         liga/desliga modo de calibracao\n"
           "  F11        tela cheia\n"
           "  Esc        sair\n"
           "=== Modo de calibracao ===\n"
           "  W/A/S/D    mover camera no plano    Q/E  descer/subir\n"
           "  Setas      girar (yaw/pitch)        Z/X  FOV -/+\n"
           "  Shift      ajuste fino              G    grade reta/diagonal (45 graus)\n"
           "  F5         salvar                   F9   recarregar\n\n",
           app.shot_count);
}

// recarrega câmeras, luzes, zonas e sala
static void reload_shots(void)
{
    shots_free(app.shots, app.shot_count);
    app.shot_count = shots_load(app.cameras_file, app.shots, MAX_SHOTS);
    if (app.cur >= app.shot_count)
        app.cur = 0;
    printf("%d angulos de camera carregados de %s\n", app.shot_count, app.cameras_file);
    shots_load_lights(LIGHTS_FILE, app.shots, app.shot_count);
    zones_load(ZONES_FILE, app.shots, app.shot_count);
    room_load(ROOM_FILE);
}

// troca de câmera por zona
static void update_camera_zone(void)
{
    int next = zones_check(app.cur, app.player.pos);
    if (next != app.cur) {
        printf("Camera: %s -> %s\n", app.shots[app.cur].name, app.shots[next].name);
        app.cur = next;
    }
}

// alterna tela cheia
static void toggle_fullscreen(void)
{
    if (!app.fullscreen) {
        glfwGetWindowPos(app.win, &app.win_x, &app.win_y);
        glfwGetWindowSize(app.win, &app.win_w, &app.win_h);
        GLFWmonitor *mon = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(mon);
        glfwSetWindowMonitor(app.win, mon, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        glfwSetWindowMonitor(app.win, NULL, app.win_x, app.win_y, app.win_w, app.win_h, 0);
    }
    app.fullscreen = !app.fullscreen;
}

// teclas de ação
static void key_callback(GLFWwindow *win, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;
    if (action != GLFW_PRESS)
        return;

    switch (key) {
    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(win, 1); break;
    case GLFW_KEY_F1:
        app.calib = !app.calib;
        printf("Modo de calibracao %s\n", app.calib ? "LIGADO" : "desligado");
        break;
    case GLFW_KEY_F5:
        if (app.calib)
            shots_save(app.cameras_file, app.shots, app.shot_count);
        break;
    case GLFW_KEY_F9: reload_shots(); break;
    case GLFW_KEY_E:
        if (!app.calib)
            scene_interact(app.player.pos);
        break;
    case GLFW_KEY_G:
        if (app.calib)
            calib_toggle_diagonal();
        break;
    case GLFW_KEY_F11: toggle_fullscreen(); break;
    case GLFW_KEY_TAB:
        if (app.shot_count > 0)
            app.cur = (app.cur + 1) % app.shot_count;
        break;
    default:
        if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9 && key - GLFW_KEY_1 < app.shot_count)
            app.cur = key - GLFW_KEY_1;
        break;
    }
}

// atualiza o título da janela
static void update_title(void)
{
    char title[256];
    if (app.shot_count == 0) {
        snprintf(title, sizeof title, "CG AB2 - sem cameras (veja %s)", app.cameras_file);
    } else {
        const CameraShot *s = &app.shots[app.cur];
        if (app.calib)
            snprintf(title, sizeof title,
                     "[CALIBRACAO] %s | eye (%.2f, %.2f, %.2f) yaw %.1f pitch %.1f fov %.1f | grade %s",
                     s->name, s->eye.x, s->eye.y, s->eye.z, s->yaw, s->pitch, s->fov,
                     calib_is_diagonal() ? "45" : "reta");
        else
            snprintf(title, sizeof title, "CG AB2 - Lesson Room | %s", s->name);
    }
    glfwSetWindowTitle(app.win, title);
}

// viewport 4:3 centralizado
static void viewport_4x3(int fb_w, int fb_h, int *x, int *y, int *w, int *h)
{
    if ((float)fb_w / fb_h > ASPECT) {
        *h = fb_h;
        *w = (int)(fb_h * ASPECT);
    } else {
        *w = fb_w;
        *h = (int)(fb_w / ASPECT);
    }
    *x = (fb_w - *w) / 2;
    *y = (fb_h - *h) / 2;
}

// desenha um quadro
static void render(void)
{
    // limpa a tela
    int fb_w, fb_h;
    glfwGetFramebufferSize(app.win, &fb_w, &fb_h);
    if (fb_w == 0 || fb_h == 0)
        return;

    glViewport(0, 0, fb_w, fb_h);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (app.shot_count == 0)
        return;

    // câmera atual
    int vx, vy, vw, vh;
    viewport_4x3(fb_w, fb_h, &vx, &vy, &vw, &vh);
    glViewport(vx, vy, vw, vh);

    const CameraShot *shot = &app.shots[app.cur];
    mat4 view = shot_view(shot);
    mat4 proj = shot_projection(shot, ASPECT);

    // fundo, sombras e cena
    background_draw(shot->bg_tex, shot->light.bg_bright);
    scene_draw_shadows(&view, &proj, app.player.pos);
    scene_draw_behind(&view, &proj);
    renderer_begin(&view, &proj, shot->eye, &shot->light);
    scene_draw();
    player_draw(&app.player);
    // debug da calibração
    if (app.calib) {
        calib_draw(&view, &proj);
        zones_draw_debug(&view, &proj, app.cur);
        room_draw_debug(&view, &proj);
    }

    // salva captura
    if (app.screenshot_path) {
        screenshot_save(app.screenshot_path, vx, vy, vw, vh);
        app.screenshot_path = NULL;
    }
}

int main(int argc, char **argv)
{
    // opções de linha de comando
    const char *auto_shot = NULL;
    int start_cam = 0;
    vec3 start_pos = v3(0.5f, 0, -2.5f);
    float start_yaw = 0.0f;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--calib")) app.calib = 1;
        else if (!strcmp(argv[i], "--player") && i + 3 < argc) {
            start_pos = v3((float)atof(argv[i + 1]), 0, (float)atof(argv[i + 2]));
            start_yaw = (float)atof(argv[i + 3]);
            i += 3;
        }
        else if (!strcmp(argv[i], "--cam") && i + 1 < argc) start_cam = atoi(argv[++i]) - 1;
        else if (!strcmp(argv[i], "--screenshot") && i + 1 < argc) auto_shot = argv[++i];
        else if (!strcmp(argv[i], "--cameras") && i + 1 < argc) app.cameras_file = argv[++i];
    }
    if (!app.cameras_file)
        app.cameras_file = DEFAULT_CAMERAS_FILE;

    // cria janela opengl 3.3
    if (!glfwInit()) {
        fprintf(stderr, "Erro: glfwInit falhou\n");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    app.win = glfwCreateWindow(WINDOW_W, WINDOW_H, "CG AB2", NULL, NULL);
    if (!app.win) {
        fprintf(stderr, "Erro: nao consegui criar a janela OpenGL 3.3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(app.win);
    glfwSwapInterval(1);
    glfwSetKeyCallback(app.win, key_callback);

    // carrega funções do opengl
    if (!gladLoadGL(glfwGetProcAddress)) {
        fprintf(stderr, "Erro: gladLoadGL falhou\n");
        return 1;
    }
    printf("OpenGL %s | %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // inicializa módulos
    if (!background_init() || !renderer_init() || !shadow_init()) {
        fprintf(stderr, "Erro: shaders nao carregados (rode o programa a partir da pasta do projeto)\n");
        return 1;
    }
    calib_init();
    materials_init();
    scene_init();
    player_init(&app.player, start_pos);
    // loop da música de fundo
    if (!auto_shot && sound_init())
        sound_loop(SND_MUSIC, 1);
    app.player.yaw = start_yaw;
    reload_shots();
    if (start_cam >= 0 && start_cam < app.shot_count)
        app.cur = start_cam;
    print_controls();

    // loop principal
    double last = glfwGetTime();
    while (!glfwWindowShouldClose(app.win)) {
        double now = glfwGetTime();
        float dt = (float)(now - last);
        last = now;

        // calibração ou movimento do personagem
        glfwPollEvents();
        if (app.shot_count > 0 && app.calib)
            calib_update(app.win, &app.shots[app.cur], dt);
        else {
            player_update(&app.player, app.win, dt);
            update_camera_zone();
        }
        scene_update(dt);

        // desenha e apresenta o quadro
        if (auto_shot)
            app.screenshot_path = auto_shot;
        render();
        update_title();
        if (auto_shot)
            break;
        glfwSwapBuffers(app.win);
    }

    // finalização
    sound_shutdown();
    shots_free(app.shots, app.shot_count);
    glfwTerminate();
    return 0;
}
