#version 330 core

// cor das zonas por uniform

in vec3 v_color;
out vec4 frag_color;

uniform vec3 u_color;

void main()
{
    frag_color = vec4(u_color * v_color, 1.0);
}
