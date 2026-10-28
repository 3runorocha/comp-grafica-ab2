#include "background.h"
#include "shader.h"

static GLuint prog;
static GLuint vao;   // vao vazio

// carrega o shader do fundo
int background_init(void)
{
    prog = shader_load("shaders/background.vert", "shaders/background.frag");
    glGenVertexArrays(1, &vao);
    return prog != 0;
}

// desenha o fundo em tela cheia
void background_draw(GLuint tex, float brightness)
{
    // desliga a profundidade
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // ativa shader e textura
    glUseProgram(prog);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    shader_set_int(prog, "u_tex", 0);
    shader_set_float(prog, "u_brightness", brightness);

    // triângulo de tela cheia
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // religa a profundidade
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}
