#version 330 core

// fogo procedural com ruido fractal

in vec2 v_uv;
out vec4 frag_color;

uniform float u_time;
uniform float u_seed;        // semente da chama
uniform float u_intensity;   // intensidade
uniform int u_mode;          // 0 chama, 1 brilho

// hash pseudoaleatorio
float hash(vec2 p)
{
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

// ruido de valor
float noise(vec2 p)
{
    vec2 i = floor(p), f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i), hash(i + vec2(1, 0)), u.x),
               mix(hash(i + vec2(0, 1)), hash(i + vec2(1, 1)), u.x), u.y);
}

// ruido fractal
float fbm(vec2 p)
{
    float v = 0.0, a = 0.5;
    for (int i = 0; i < 5; i++) {
        v += a * noise(p);
        p = p * 2.03 + vec2(1.7, 9.2);
        a *= 0.5;
    }
    return v;
}

void main()
{
    if (u_mode == 1) {
        // brilho laranja na base
        vec2 d = (v_uv - vec2(0.5, 0.3)) * vec2(1.0, 1.7);
        float g = exp(-dot(d, d) * 9.0);
        frag_color = vec4(vec3(1.0, 0.42, 0.1) * g * 0.45 * u_intensity, 1.0);
        return;
    }

    float x = v_uv.x * 2.0 - 1.0;   // horizontal centrado
    float y = v_uv.y;               // altura

    // ruido fractal subindo
    vec2 q = vec2(x * 1.6 + u_seed * 7.3, y * 2.4 - u_time * 1.8);
    float n = fbm(q);
    float n2 = fbm(q * 2.1 + vec2(3.1, -u_time * 1.2));

    // formato da chama
    float xw = x + (n - 0.5) * 1.1 * y;
    float width = mix(0.8, 0.1, y);
    float body = 1.0 - smoothstep(width * 0.45, width, abs(xw));

    // calor da chama
    float heat = body * (1.15 - y) * 1.3 + (n2 - 0.5) * 0.55;
    heat *= smoothstep(0.0, 0.15, y);           // base suave
    heat = clamp(heat, 0.0, 1.0);

    // rampa de cor do calor
    vec3 col = vec3(0.55, 0.06, 0.0) * smoothstep(0.05, 0.35, heat);
    col = mix(col, vec3(1.0, 0.42, 0.05), smoothstep(0.3, 0.62, heat));
    col = mix(col, vec3(1.0, 0.85, 0.5), smoothstep(0.62, 0.92, heat));
    float alpha = smoothstep(0.08, 0.4, heat);

    frag_color = vec4(col * alpha * u_intensity, 1.0);
}
