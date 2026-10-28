#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

// lê o arquivo inteiro
static char *read_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Erro: nao consegui abrir '%s'\n", path);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc((size_t)size + 1);
    size_t got = fread(buf, 1, (size_t)size, f);
    buf[got] = '\0';
    fclose(f);
    return buf;
}

// compila um shader
static GLuint compile(GLenum type, const char *path)
{
    char *src = read_file(path);
    if (!src)
        return 0;

    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, (const char **)&src, NULL);
    glCompileShader(sh);
    free(src);

    // verifica erro de compilação
    GLint ok;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(sh, sizeof log, NULL, log);
        fprintf(stderr, "Erro compilando '%s':\n%s\n", path, log);
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

// carrega e liga o shader
GLuint shader_load(const char *vs_path, const char *fs_path)
{
    GLuint vs = compile(GL_VERTEX_SHADER, vs_path);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fs_path);
    if (!vs || !fs)
        return 0;

    // liga o programa
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);

    // verifica erro de ligação
    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(prog, sizeof log, NULL, log);
        fprintf(stderr, "Erro ligando '%s' + '%s':\n%s\n", vs_path, fs_path, log);
        glDeleteProgram(prog);
        return 0;
    }
    return prog;
}

// uniform mat4
void shader_set_mat4(GLuint prog, const char *name, const mat4 *m)
{
    glUniformMatrix4fv(glGetUniformLocation(prog, name), 1, GL_FALSE, m->m);
}

// uniform vec3
void shader_set_vec3(GLuint prog, const char *name, vec3 v)
{
    glUniform3f(glGetUniformLocation(prog, name), v.x, v.y, v.z);
}

// uniform float
void shader_set_float(GLuint prog, const char *name, float f)
{
    glUniform1f(glGetUniformLocation(prog, name), f);
}

// uniform int
void shader_set_int(GLuint prog, const char *name, int i)
{
    glUniform1i(glGetUniformLocation(prog, name), i);
}
