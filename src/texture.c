#include "texture.h"
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

// carrega a textura
GLuint texture_load(const char *path)
{
    int w, h, channels;
    // lê a imagem invertida
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(path, &w, &h, &channels, 4);
    if (!data) {
        fprintf(stderr, "Erro: textura '%s' nao carregada (%s)\n", path, stbi_failure_reason());
        return 0;
    }

    // envia para a gpu com mipmaps
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(data);
    return tex;
}

// textura de cor sólida
GLuint texture_solid(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned char px[4] = { r, g, b, 255 };
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return tex;
}

// salva captura de tela
int screenshot_save(const char *path, int x, int y, int w, int h)
{
    unsigned char *px = malloc((size_t)w * h * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, px);
    stbi_flip_vertically_on_write(1);
    int ok = stbi_write_png(path, w, h, 3, px, w * 3);
    free(px);
    if (ok)
        printf("Captura salva em %s\n", path);
    else
        fprintf(stderr, "Erro: nao consegui salvar %s\n", path);
    return ok;
}
