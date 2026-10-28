#include "calib.h"
#include "mesh.h"
#include "shader.h"
#include <math.h>

#define GRID_HALF 16                       // azulejos por lado
#define GRID_LINES (2 * (2 * GRID_HALF + 1))
#define GRID_VERTS (GRID_LINES * 2)

static GLuint prog;
static Mesh grid, axes;
static int grid_diagonal;   // 1 = grade a 45°

// adiciona uma linha colorida
static float *push_line(float *p, vec3 a, vec3 b, vec3 color)
{
    vec3 pts[2] = { a, b };
    for (int i = 0; i < 2; i++) {
        *p++ = pts[i].x; *p++ = pts[i].y; *p++ = pts[i].z;
        *p++ = color.x;  *p++ = color.y;  *p++ = color.z;
    }
    return p;
}

// cria a grade e os eixos
void calib_init(void)
{
    prog = shader_load("shaders/lines.vert", "shaders/lines.frag");

    static float verts[GRID_VERTS * 6];
    float axis_verts[6 * 6];
    float *p = verts;
    float ext = GRID_HALF * TILE_SIZE;
    vec3 yellow = v3(1.0f, 0.85f, 0.1f);

    // linhas da grade
    for (int i = -GRID_HALF; i <= GRID_HALF; i++) {
        float c = i * TILE_SIZE;
        p = push_line(p, v3(c, 0, -ext), v3(c, 0, ext), yellow);   // paralela a z
        p = push_line(p, v3(-ext, 0, c), v3(ext, 0, c), yellow);   // paralela a x
    }
    grid = mesh_create_lines(verts, GRID_VERTS);

    // eixos do mundo
    p = axis_verts;
    p = push_line(p, v3(0, 0.001f, 0), v3(1, 0.001f, 0), v3(1, 0.1f, 0.1f));
    p = push_line(p, v3(0, 0.001f, 0), v3(0, 0.001f, 1), v3(0.2f, 0.4f, 1));
    p = push_line(p, v3(0, 0, 0), v3(0, 2, 0), v3(0.1f, 1, 0.2f));
    axes = mesh_create_lines(axis_verts, 6);
}

// alterna grade diagonal
void calib_toggle_diagonal(void)
{
    grid_diagonal = !grid_diagonal;
}

// grade está diagonal
int calib_is_diagonal(void)
{
    return grid_diagonal;
}

// move a câmera pelo teclado
void calib_update(GLFWwindow *win, CameraShot *shot, float dt)
{
    // velocidades com ajuste fino
    float k = (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 0.1f : 1.0f;
    float move = 1.0f * k * dt;    // metros por segundo
    float turn = 20.0f * k * dt;   // graus por segundo
    float zoom = 10.0f * k * dt;

    vec3 fwd = shot_forward(shot);
    vec3 flat = v3_normalize(v3(fwd.x, 0, fwd.z));
    vec3 right = v3_normalize(v3_cross(flat, v3(0, 1, 0)));

    // translação
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) shot->eye = v3_add(shot->eye, v3_scale(flat, move));
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) shot->eye = v3_sub(shot->eye, v3_scale(flat, move));
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) shot->eye = v3_add(shot->eye, v3_scale(right, move));
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) shot->eye = v3_sub(shot->eye, v3_scale(right, move));
    if (glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS) shot->eye.y += move;
    if (glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS) shot->eye.y -= move;

    // rotação e fov
    if (glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS)  shot->yaw -= turn;
    if (glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS) shot->yaw += turn;
    if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)    shot->pitch += turn;
    if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS)  shot->pitch -= turn;
    if (glfwGetKey(win, GLFW_KEY_Z) == GLFW_PRESS)     shot->fov -= zoom;
    if (glfwGetKey(win, GLFW_KEY_X) == GLFW_PRESS)     shot->fov += zoom;

    // limita pitch e fov
    shot->pitch = fmaxf(-89.0f, fminf(89.0f, shot->pitch));
    shot->fov = fmaxf(10.0f, fminf(120.0f, shot->fov));
}

// desenha eixos e grade
void calib_draw(const mat4 *view, const mat4 *proj)
{
    // eixos por cima de tudo
    glDisable(GL_DEPTH_TEST);
    glUseProgram(prog);
    mat4 vp = m4_mul(*proj, *view);
    shader_set_mat4(prog, "u_vp", &vp);
    mesh_draw(&axes);

    // grade, girada se diagonal
    mat4 grid_vp = grid_diagonal ? m4_mul(vp, m4_rotate(deg2rad(45.0f), v3(0, 1, 0))) : vp;
    shader_set_mat4(prog, "u_vp", &grid_vp);
    mesh_draw(&grid);
    glEnable(GL_DEPTH_TEST);
}
