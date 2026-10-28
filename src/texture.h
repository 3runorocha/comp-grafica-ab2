#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/gl.h>

// carrega a textura de um arquivo
GLuint texture_load(const char *path);

// textura 1x1 de cor sólida
GLuint texture_solid(unsigned char r, unsigned char g, unsigned char b);

// salva captura de tela em png
int screenshot_save(const char *path, int x, int y, int w, int h);

#endif
