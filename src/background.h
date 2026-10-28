#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <glad/gl.h>

// carrega o shader do fundo
int background_init(void);
// desenha o fundo em tela cheia
void background_draw(GLuint tex, float brightness);

#endif
