#include "renderer.h"
#include "shader.h"
#include <stdio.h>

static GLuint prog;

// carrega o shader iluminado
int renderer_init(void)
{
    prog = shader_load("shaders/lit.vert", "shaders/lit.frag");
    return prog != 0;
}

// começa o quadro
void renderer_begin(const mat4 *view, const mat4 *proj, vec3 eye, const SceneLight *light)
{
    // câmera
    glUseProgram(prog);
    shader_set_mat4(prog, "u_view", view);
    shader_set_mat4(prog, "u_proj", proj);
    shader_set_vec3(prog, "u_view_pos", eye);

    // luz de base e texturas
    shader_set_vec3(prog, "u_light_dir", v3_normalize(light->dir));
    shader_set_vec3(prog, "u_light_color", light->color);
    shader_set_vec3(prog, "u_ambient", light->ambient);
    shader_set_int(prog, "u_tex", 0);
    shader_set_int(prog, "u_emit_tex", 1);
    shader_set_int(prog, "u_num_points", 0);
}

// envia as luzes pontuais
void renderer_set_point_lights(const PointLight *lights, int count)
{
    if (count > MAX_POINT_LIGHTS)
        count = MAX_POINT_LIGHTS;
    glUseProgram(prog);
    shader_set_int(prog, "u_num_points", count);
    for (int i = 0; i < count; i++) {
        char name[32];
        snprintf(name, sizeof name, "u_point_pos[%d]", i);
        shader_set_vec3(prog, name, lights[i].pos);
        snprintf(name, sizeof name, "u_point_color[%d]", i);
        shader_set_vec3(prog, name, lights[i].color);
        snprintf(name, sizeof name, "u_point_range[%d]", i);
        shader_set_float(prog, name, lights[i].range);
        snprintf(name, sizeof name, "u_point_dir[%d]", i);
        shader_set_vec3(prog, name, lights[i].dir);
        snprintf(name, sizeof name, "u_point_spot[%d]", i);
        shader_set_float(prog, name, lights[i].spot_cos);
    }
}

// desenha uma malha com material
void renderer_draw(const Mesh *mesh, const mat4 *model, const Material *mat)
{
    // envia o material
    glUseProgram(prog);
    shader_set_mat4(prog, "u_model", model);
    shader_set_vec3(prog, "u_mat.ambient", mat->ambient);
    shader_set_vec3(prog, "u_mat.diffuse", mat->diffuse);
    shader_set_vec3(prog, "u_mat.specular", mat->specular);
    shader_set_float(prog, "u_mat.shininess", mat->shininess);
    shader_set_vec3(prog, "u_mat.emissive", mat->emissive);
    shader_set_int(prog, "u_use_tex", mat->tex != 0);
    shader_set_int(prog, "u_additive", mat->additive);
    // liga as texturas
    if (mat->tex) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mat->tex);
    }
    shader_set_int(prog, "u_use_emit_tex", mat->emit_tex != 0);
    if (mat->emit_tex) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mat->emit_tex);
        glActiveTexture(GL_TEXTURE0);
    }
    // desenho aditivo ou normal
    if (mat->additive) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(GL_FALSE);
        mesh_draw(mesh);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    } else {
        mesh_draw(mesh);
    }
}
