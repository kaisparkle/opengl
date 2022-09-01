#version 460 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

layout (location = 0) out vec2 fUV;

layout (location = 0) uniform mat4 matrix_viewproj;
layout (location = 1) uniform mat4 matrix_model;

void main() {
    fUV = vUV;
	mat4 transformMatrix = (matrix_viewproj * matrix_model);
    gl_Position = transformMatrix * vec4(vPos, 1.0f);
}