#include "shadow.h"
#include "shader.h"

static GLuint prog, vao, vbo;

// carrega shader e retângulo da sombra
int shadow_init(void)
{
    prog = shader_load("shaders/shadow.vert", "shaders/shadow.frag");
    // retângulo unitário em xz
    static const float corners[] = { -1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1 };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof corners, corners, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return prog != 0;
}

// prepara o estado das sombras
void shadow_begin(const mat4 *view, const mat4 *proj)
{
    mat4 vp = m4_mul(*proj, *view);
    glUseProgram(prog);
    shader_set_mat4(prog, "u_vp", &vp);
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);   // blend multiplicativo
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glBindVertexArray(vao);
}

// desenha uma sombra elíptica
void shadow_draw(vec3 center, float rx, float rz, float strength)
{
    shader_set_vec3(prog, "u_center", v3(center.x, 0.0f, center.z));
    glUniform2f(glGetUniformLocation(prog, "u_radius"), rx, rz);
    shader_set_float(prog, "u_strength", strength);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// restaura o estado
void shadow_end(void)
{
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
