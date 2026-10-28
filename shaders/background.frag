#version 330 core

// imagem de fundo com brilho ajustavel

in vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_tex;
uniform float u_brightness;   // fator de brilho

void main()
{
    frag_color = vec4(texture(u_tex, v_uv).rgb * u_brightness, 1.0);
}
