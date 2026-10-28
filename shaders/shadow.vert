#version 330 core

// retangulo da sombra no chao

layout(location = 0) in vec2 a_corner;

uniform mat4 u_vp;
uniform vec3 u_center;    // centro no chao
uniform vec2 u_radius;    // meia-largura em x e z

out vec2 v_uv;

void main()
{
    v_uv = a_corner;
    vec3 p = u_center + vec3(a_corner.x * u_radius.x, 0.005, a_corner.y * u_radius.y);
    gl_Position = u_vp * vec4(p, 1.0);
}
