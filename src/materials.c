#include "materials.h"
#include "texture.h"

// madeira envernizada
Material mat_wood_varnish = {
    .ambient = { 0.95f, 0.95f, 0.95f },
    .diffuse = { 0.95f, 0.95f, 0.95f },
    .specular = { 0.35f, 0.32f, 0.28f },
    .shininess = 48.0f,
};

// madeira crua
Material mat_wood_raw = {
    .ambient = { 1.0f, 1.0f, 1.0f },
    .diffuse = { 1.0f, 1.0f, 1.0f },
    .specular = { 0.04f, 0.04f, 0.04f },
    .shininess = 6.0f,
};

// madeira interna escura
Material mat_wood_inner = {
    .ambient = { 0.5f, 0.45f, 0.4f },
    .diffuse = { 0.5f, 0.45f, 0.4f },
    .specular = { 0.02f, 0.02f, 0.02f },
    .shininess = 4.0f,
};

// latão
Material mat_brass = {
    .ambient = { 0.40f, 0.29f, 0.11f },
    .diffuse = { 0.78f, 0.57f, 0.21f },
    .specular = { 0.95f, 0.85f, 0.55f },
    .shininess = 96.0f,
};

// fundo escuro de vãos
Material mat_shadow = {
    .ambient = { 0.05f, 0.04f, 0.03f },
    .diffuse = { 0.05f, 0.04f, 0.03f },
    .specular = { 0.0f, 0.0f, 0.0f },
    .shininess = 1.0f,
};

// roupa
Material mat_cloth = {
    .ambient = { 0.25f, 0.3f, 0.4f },
    .diffuse = { 0.25f, 0.3f, 0.4f },
    .specular = { 0.2f, 0.2f, 0.2f },
    .shininess = 16.0f,
};

// metal pintado
Material mat_metal_painted = {
    .ambient = { 0.9f, 0.9f, 0.9f },
    .diffuse = { 0.9f, 0.9f, 0.9f },
    .specular = { 0.45f, 0.45f, 0.45f },
    .shininess = 40.0f,
};

// aço
Material mat_steel = {
    .ambient = { 0.30f, 0.30f, 0.32f },
    .diffuse = { 0.50f, 0.50f, 0.52f },
    .specular = { 0.85f, 0.85f, 0.85f },
    .shininess = 80.0f,
};

// borracha
Material mat_rubber = {
    .ambient = { 0.05f, 0.05f, 0.05f },
    .diffuse = { 0.06f, 0.06f, 0.06f },
    .specular = { 0.08f, 0.08f, 0.08f },
    .shininess = 8.0f,
};

// madeira da cadeira
Material mat_wood_chair = {
    .ambient = { 0.65f, 0.55f, 0.5f },
    .diffuse = { 0.65f, 0.55f, 0.5f },
    .specular = { 0.25f, 0.23f, 0.2f },
    .shininess = 32.0f,
};

// couro
Material mat_leather = {
    .ambient = { 1.0f, 1.0f, 1.0f },
    .diffuse = { 1.0f, 1.0f, 1.0f },
    .specular = { 0.18f, 0.15f, 0.14f },
    .shininess = 12.0f,
};

// madeira do cavalete
Material mat_wood_easel = {
    .ambient = { 0.9f, 0.82f, 0.72f },
    .diffuse = { 0.9f, 0.82f, 0.72f },
    .specular = { 0.12f, 0.11f, 0.1f },
    .shininess = 16.0f,
};

// lousa
Material mat_slate = {
    .ambient = { 1.0f, 1.0f, 1.0f },
    .diffuse = { 1.0f, 1.0f, 1.0f },
    .specular = { 0.08f, 0.08f, 0.08f },
    .shininess = 10.0f,
};

// giz
Material mat_chalk = {
    .ambient = { 0.9f, 0.9f, 0.88f },
    .diffuse = { 0.9f, 0.9f, 0.88f },
    .specular = { 0.0f, 0.0f, 0.0f },
    .shininess = 1.0f,
};

// feltro do apagador
Material mat_felt = {
    .ambient = { 0.3f, 0.3f, 0.32f },
    .diffuse = { 0.3f, 0.3f, 0.32f },
    .specular = { 0.0f, 0.0f, 0.0f },
    .shininess = 1.0f,
};

// latão envelhecido
Material mat_brass_aged = {
    .ambient = { 0.30f, 0.23f, 0.10f },
    .diffuse = { 0.60f, 0.46f, 0.22f },
    .specular = { 0.80f, 0.70f, 0.45f },
    .shininess = 64.0f,
};

// mapa do globo
Material mat_globe_map = {
    .ambient = { 1.0f, 1.0f, 1.0f },
    .diffuse = { 1.0f, 1.0f, 1.0f },
    .specular = { 0.35f, 0.33f, 0.3f },
    .shininess = 40.0f,
};

// cerâmica do abajur
Material mat_ceramic = {
    .ambient = { 0.10f, 0.22f, 0.18f },
    .diffuse = { 0.12f, 0.30f, 0.24f },
    .specular = { 0.7f, 0.7f, 0.7f },
    .shininess = 90.0f,
};

// cúpula do abajur
Material mat_lampshade = {
    .ambient = { 1.0f, 1.0f, 1.0f },
    .diffuse = { 1.0f, 1.0f, 1.0f },
    .specular = { 0.03f, 0.03f, 0.03f },
    .shininess = 4.0f,
    .emissive = { 0.0f, 0.0f, 0.0f },   // acende com o abajur
};

// lâmpada
Material mat_bulb = {
    .ambient = { 0.8f, 0.8f, 0.75f },
    .diffuse = { 0.8f, 0.8f, 0.75f },
    .specular = { 0.9f, 0.9f, 0.9f },
    .shininess = 120.0f,
    .emissive = { 0.0f, 0.0f, 0.0f },   // acende com o abajur
};

// lenha carbonizada
Material mat_log = {
    .ambient = { 1.0f, 1.0f, 1.0f },
    .diffuse = { 1.0f, 1.0f, 1.0f },
    .specular = { 0.05f, 0.05f, 0.05f },
    .shininess = 6.0f,
    .emissive = { 0.0f, 0.0f, 0.0f },   // acende com o fogo
};

// carvão em brasa
Material mat_coal = {
    .ambient = { 0.6f, 0.6f, 0.6f },
    .diffuse = { 0.6f, 0.6f, 0.6f },
    .specular = { 0.1f, 0.1f, 0.1f },
    .shininess = 10.0f,
    .emissive = { 0.0f, 0.0f, 0.0f },   // acende com o fogo
};

// ferro da grelha
Material mat_iron = {
    .ambient = { 0.12f, 0.12f, 0.13f },
    .diffuse = { 0.16f, 0.16f, 0.17f },
    .specular = { 0.45f, 0.45f, 0.45f },
    .shininess = 30.0f,
};

// carrega as texturas dos materiais
void materials_init(void)
{
    GLuint varnish = texture_load("assets/textures/wood_varnish.png");
    GLuint raw = texture_load("assets/textures/wood_raw.png");
    mat_wood_varnish.tex = varnish;
    mat_wood_raw.tex = raw;
    mat_wood_inner.tex = raw;
    mat_metal_painted.tex = texture_load("assets/textures/metal_painted.png");
    mat_wood_chair.tex = varnish;
    mat_leather.tex = texture_load("assets/textures/leather.png");
    mat_wood_easel.tex = raw;
    mat_slate.tex = texture_load("assets/textures/chalkboard.png");
    mat_globe_map.tex = texture_load("assets/textures/globe_map.png");
    GLuint charred = texture_load("assets/textures/charred_log.png");
    GLuint charred_glow = texture_load("assets/textures/charred_log_glow.png");
    mat_log.tex = mat_coal.tex = charred;
    mat_log.emit_tex = charred_glow;   // brilho das rachaduras
    mat_lampshade.tex = texture_load("assets/textures/linen.png");
}
