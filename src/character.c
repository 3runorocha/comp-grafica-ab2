#include "character.h"
#include "texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// lista de floats dinâmica
typedef struct {
    float *v;
    int count, cap;
} FloatList;

// adiciona um float
static void push(FloatList *l, float x)
{
    if (l->count == l->cap) {
        l->cap = l->cap ? l->cap * 2 : 1024;
        l->v = realloc(l->v, (size_t)l->cap * sizeof(float));
    }
    l->v[l->count++] = x;
}

// lê a hierarquia das partes
static int read_skeleton(const char *path, int *parent, vec3 *rel)
{
    FILE *f = fopen(path, "r");
    if (!f)
        return 0;
    char line[256];
    int count = 0;
    while (fgets(line, sizeof line, f)) {
        int i, p;
        float x, y, z;
        if (line[0] == '#' || sscanf(line, "%d %d %f %f %f", &i, &p, &x, &y, &z) != 5)
            continue;
        if (i < 0 || i >= CHAR_MAX_PARTS)
            continue;
        parent[i] = p;
        rel[i] = v3(x, y, z);
        if (i + 1 > count)
            count = i + 1;
    }
    fclose(f);
    return count;
}

// lê o obj separando por parte
static int read_obj(const char *path, FloatList *part_verts)
{
    FILE *f = fopen(path, "r");
    if (!f)
        return 0;
    FloatList pos = { 0 }, uv = { 0 }, nrm = { 0 };
    int part = 0;
    char line[512];

    while (fgets(line, sizeof line, f)) {
        float a, b, c;
        if (strncmp(line, "o part_", 7) == 0) {
            part = atoi(line + 7);
            if (part < 0 || part >= CHAR_MAX_PARTS)
                part = 0;
        } else if (sscanf(line, "v %f %f %f", &a, &b, &c) == 3) {
            push(&pos, a); push(&pos, b); push(&pos, c);
        } else if (sscanf(line, "vt %f %f", &a, &b) == 2) {
            push(&uv, a); push(&uv, b);
        } else if (sscanf(line, "vn %f %f %f", &a, &b, &c) == 3) {
            push(&nrm, a); push(&nrm, b); push(&nrm, c);
        } else if (line[0] == 'f' && line[1] == ' ') {
            int vi[3], ti[3], ni[3];
            if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vi[0], &ti[0], &ni[0],
                       &vi[1], &ti[1], &ni[1], &vi[2], &ti[2], &ni[2]) != 9)
                continue;
            // vértices posição, normal, uv
            for (int k = 0; k < 3; k++) {
                const float *p = &pos.v[(vi[k] - 1) * 3], *n = &nrm.v[(ni[k] - 1) * 3], *t = &uv.v[(ti[k] - 1) * 2];
                push(&part_verts[part], p[0]); push(&part_verts[part], p[1]); push(&part_verts[part], p[2]);
                push(&part_verts[part], n[0]); push(&part_verts[part], n[1]); push(&part_verts[part], n[2]);
                push(&part_verts[part], t[0]); push(&part_verts[part], t[1]);
            }
        }
    }
    fclose(f);
    free(pos.v);
    free(uv.v);
    free(nrm.v);
    return 1;
}

// carrega o personagem
int character_load(Character *c, const char *dir, float scale, float facing_deg)
{
    memset(c, 0, sizeof *c);
    char path[512];
    int parent[CHAR_MAX_PARTS];
    vec3 rel[CHAR_MAX_PARTS];
    for (int i = 0; i < CHAR_MAX_PARTS; i++)
        parent[i] = -1;

    snprintf(path, sizeof path, "%s/skeleton.txt", dir);
    c->part_count = read_skeleton(path, parent, rel);
    FloatList verts[CHAR_MAX_PARTS] = { { 0 } };
    snprintf(path, sizeof path, "%s/model.obj", dir);
    if (c->part_count == 0 || !read_obj(path, verts)) {
        fprintf(stderr, "Aviso: personagem em '%s' nao encontrado (veja assets/characters/README.md)\n", dir);
        return 0;
    }

    // textura sem mipmaps
    snprintf(path, sizeof path, "%s/texture.png", dir);
    c->mat = (Material){ .ambient = { 1, 1, 1 }, .diffuse = { 1, 1, 1 },
                         .specular = { 0.12f, 0.12f, 0.12f }, .shininess = 16.0f };
    c->mat.tex = texture_load(path);
    if (c->mat.tex) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // raiz e corpo com escala
    c->root = node_group(NULL, v3(0, 0, 0));
    c->body = node_group(c->root, v3(0, 0, 0));
    c->body->scale = v3(scale, scale, scale);
    c->body->rot.y = facing_deg;

    // cria as partes
    for (int i = 0; i < c->part_count; i++) {
        Mesh m = { 0 };
        if (verts[i].count > 0)
            m = mesh_create_pnu(verts[i].v, verts[i].count / 8);
        c->parts[i] = node_mesh(NULL, m, rel[i], &c->mat);
        free(verts[i].v);
    }
    // pendura cada parte na mãe
    for (int i = 0; i < c->part_count; i++) {
        int p = parent[i];
        node_attach(p >= 0 && p < c->part_count ? c->parts[p] : c->body, c->parts[i]);
    }
    printf("Personagem '%s': %d partes\n", dir, c->part_count);
    return 1;
}
