#version 460 core

layout (location = 0) in vec3 aPos;

layout (location = 0) uniform mat4 matrix_model;

void main() {
    gl_Position = matrix_model * vec4(aPos, 1.0);
}