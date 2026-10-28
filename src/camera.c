#include "camera.h"
#include "texture.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

// direção da câmera por yaw/pitch
vec3 shot_forward(const CameraShot *s)
{
    float y = deg2rad(s->yaw), p = deg2rad(s->pitch);
    return v3(cosf(p) * cosf(y), sinf(p), cosf(p) * sinf(y));
}

// matriz de visão
mat4 shot_view(const CameraShot *s)
{
    return m4_lookat(s->eye, v3_add(s->eye, shot_forward(s)), v3(0, 1, 0));
}

// matriz de projeção
mat4 shot_projection(const CameraShot *s, float aspect)
{
    return m4_perspective(deg2rad(s->fov), aspect, 0.05f, 100.0f);
}

// carrega os ângulos de câmera
int shots_load(const char *path, CameraShot *out, int max)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Erro: nao consegui abrir '%s'\n", path);
        return 0;
    }

    // lê cada linha do arquivo
    int count = 0;
    char line[512];
    while (count < max && fgets(line, sizeof line, f)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;

        CameraShot *s = &out[count];
        memset(s, 0, sizeof *s);
        int n = sscanf(line, "%31s %255s %f %f %f %f %f %f", s->name, s->bg_path,
                       &s->eye.x, &s->eye.y, &s->eye.z, &s->yaw, &s->pitch, &s->fov);
        if (n != 8) {
            fprintf(stderr, "Aviso: linha ignorada em %s: %s", path, line);
            continue;
        }

        // luz padrão e textura do fundo
        s->light = (SceneLight){ .dir = v3(0.4f, -1.0f, -0.3f), .color = v3(1.0f, 0.92f, 0.78f),
                                 .ambient = v3(0.35f, 0.33f, 0.30f), .bg_bright = 1.0f };
        s->bg_tex = texture_load(s->bg_path);
        if (s->bg_tex) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        } else {
            fprintf(stderr, "  -> veja assets/backgrounds/README.md para obter os fundos.\n");
            s->bg_tex = texture_solid(40, 40, 48);
        }
        count++;
    }
    fclose(f);
    return count;
}

// salva os ângulos de câmera
int shots_save(const char *path, const CameraShot *shots, int count)
{
    FILE *f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "Erro: nao consegui gravar '%s'\n", path);
        return 0;
    }
    fprintf(f, "# Angulos de camera (gerado pelo modo de calibracao - F5 salva)\n");
    fprintf(f, "# nome  fundo  eye.x eye.y eye.z  yaw pitch  fov\n");
    for (int i = 0; i < count; i++) {
        const CameraShot *s = &shots[i];
        fprintf(f, "%s %s  %.3f %.3f %.3f  %.2f %.2f  %.2f\n", s->name, s->bg_path,
                s->eye.x, s->eye.y, s->eye.z, s->yaw, s->pitch, s->fov);
    }
    fclose(f);
    printf("Cameras salvas em %s\n", path);
    return 1;
}

// libera as texturas de fundo
void shots_free(CameraShot *shots, int count)
{
    for (int i = 0; i < count; i++) {
        glDeleteTextures(1, &shots[i].bg_tex);
        shots[i].bg_tex = 0;
    }
}

// carrega a luz de cada câmera
void shots_load_lights(const char *path, CameraShot *shots, int count)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Aviso: sem %s, usando luz padrao\n", path);
        return;
    }
    char line[256], name[32];
    SceneLight l;
    while (fgets(line, sizeof line, f)) {
        if (line[0] == '#')
            continue;
        if (sscanf(line, "%31s %f %f %f %f %f %f %f %f %f %f", name, &l.dir.x, &l.dir.y, &l.dir.z,
                   &l.color.x, &l.color.y, &l.color.z, &l.ambient.x, &l.ambient.y, &l.ambient.z,
                   &l.bg_bright) != 11)
            continue;
        // aplica à câmera de mesmo nome
        for (int i = 0; i < count; i++)
            if (strcmp(shots[i].name, name) == 0)
                shots[i].light = l;
    }
    fclose(f);
}
