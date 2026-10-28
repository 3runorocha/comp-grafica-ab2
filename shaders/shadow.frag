#version 330 core

// sombra blob multiplicativa

in vec2 v_uv;
out vec4 frag_color;

uniform float u_strength;   // forca no centro

void main()
{
    float d = length(v_uv);
    float shade = u_strength * (1.0 - smoothstep(0.15, 1.0, d));
    frag_color = vec4(vec3(1.0 - shade), 1.0);
}
