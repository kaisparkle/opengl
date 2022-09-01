#pragma once

#include <vector>
#include <gl/vertex.h>
#include <gl/texture.h>
#include <gl/shader.h>

namespace GLRenderer {
    struct Mesh {
        unsigned int VAO, VBO, EBO;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        Texture *texture;
        PBRTexture *pbrTexture;

        void setup_mesh();

        void draw_mesh(glm::mat4 modelMatrix, Shader *shader);
    };
}