#version 300 es
#pragma STDGL invariant(all)
layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec3 o_color;
out vec3 o_normal;
out vec3 o_position;

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
    o_color = color;
    o_normal = mat3(modelMatrix) * normal;
    o_position = mat3(modelMatrix) * position;
}