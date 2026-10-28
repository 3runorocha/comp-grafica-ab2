#version 330 core

// iluminacao blinn-phong com luzes pontuais

#define MAX_POINT_LIGHTS 4

in vec3 v_world_pos;
in vec3 v_normal;
in vec2 v_uv;

out vec4 frag_color;

// material
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    vec3 emissive;
};

uniform Material u_mat;
uniform sampler2D u_tex;
uniform int u_use_tex;
uniform int u_additive;      // brilho aditivo
uniform sampler2D u_emit_tex;  // mapa de emissao
uniform int u_use_emit_tex;

uniform vec3 u_light_dir;     // direcao da luz
uniform vec3 u_light_color;
uniform vec3 u_ambient;       // luz ambiente
uniform vec3 u_view_pos;

uniform int u_num_points;
uniform vec3 u_point_pos[MAX_POINT_LIGHTS];
uniform vec3 u_point_color[MAX_POINT_LIGHTS];
uniform float u_point_range[MAX_POINT_LIGHTS];   // alcance
uniform vec3 u_point_dir[MAX_POINT_LIGHTS];      // direcao do cone
uniform float u_point_spot[MAX_POINT_LIGHTS];    // abertura do cone

// difusa + especular
vec3 shade(vec3 n, vec3 v, vec3 l, vec3 color, vec3 base)
{
    vec3 h = normalize(l + v);
    vec3 diffuse = color * u_mat.diffuse * base * max(dot(n, l), 0.0);
    vec3 specular = color * u_mat.specular * pow(max(dot(n, h), 0.0), u_mat.shininess);
    return diffuse + specular;
}

void main()
{
    vec3 base = (u_use_tex != 0) ? texture(u_tex, v_uv).rgb : vec3(1.0);

    vec3 n = normalize(v_normal);
    vec3 v = normalize(u_view_pos - v_world_pos);

    // ambiente + luz direcional
    vec3 color = u_ambient * u_mat.ambient * base;
    color += shade(n, v, normalize(-u_light_dir), u_light_color, base);

    // luzes pontuais
    for (int i = 0; i < u_num_points; i++) {
        vec3 to_light = u_point_pos[i] - v_world_pos;
        float d = length(to_light);
        // atenuacao quadratica
        float att = clamp(1.0 - d / u_point_range[i], 0.0, 1.0);
        att *= att;
        // cone do spot
        if (u_point_spot[i] > -1.0) {
            float c = dot(-to_light / d, normalize(u_point_dir[i]));
            att *= smoothstep(u_point_spot[i], u_point_spot[i] + 0.15, c);
        }
        color += shade(n, v, to_light / d, u_point_color[i] * att, base);
    }

    // emissao
    vec3 emit_map = (u_use_emit_tex != 0) ? texture(u_emit_tex, v_uv).rgb : base;
    vec3 emission = u_mat.emissive * emit_map;
    // chamas somem nas bordas
    if (u_additive != 0) {
        float facing = abs(dot(n, v));
        emission *= facing * facing;
    }

    frag_color = vec4(color + emission, 1.0);
}
