#version 460 core

layout (location = 0) in vec2 fUV;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform sampler2D texture_diffuse;

void main() {
    outColor = texture(texture_diffuse, fUV);
}