#version 300 es
#define AMBIENT_INTENSITY 0.2f
precision highp float;

uniform vec3 directionalLightPos;

in vec3 o_color;
in vec3 o_normal;
in vec3 o_position;

out vec4 FragColor;

vec3 getDiffuse() 
{
    vec3 _dir = normalize(directionalLightPos - o_position);
    vec3 _norm = normalize(o_normal);
    return vec3(1.0f, 1.0f, 1.0f) * max(0.02f, dot(_dir, _norm));
}

void main() {
    vec3 ambient = vec3(1.0f, 1.0f, 1.0f) * AMBIENT_INTENSITY;

    vec3 resColor = o_color * (ambient + getDiffuse());
    FragColor = vec4(resColor, 1.0f);
}