#version 330 core

// billboard cilindrico da chama

layout(location = 0) in vec2 a_corner;   // canto do retangulo

uniform mat4 u_vp;
uniform vec3 u_center;     // base da chama
uniform vec2 u_size;       // largura e altura
uniform vec3 u_cam_right;  // direita da camera

out vec2 v_uv;

void main()
{
    vec3 p = u_center + u_cam_right * (a_corner.x * u_size.x) + vec3(0.0, a_corner.y * u_size.y, 0.0);
    v_uv = vec2(a_corner.x + 0.5, a_corner.y);
    gl_Position = u_vp * vec4(p, 1.0);
}
