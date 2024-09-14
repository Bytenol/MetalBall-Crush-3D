#version 300 es
precision highp float;

in vec3 o_color;

out vec4 FragColor;

void main() {
    FragColor = vec4(o_color, 1.0f);
}

