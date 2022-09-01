#include "mesh.h"

namespace GLRenderer {
    void Mesh::setup_mesh() {
        // generate IDs
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // copy vertices
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        // copy indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set up attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
        // normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
        // uvs
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, uv));
        // tangents
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));
        // bitangents
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bitangent));

        glBindVertexArray(0);
    }

    void Mesh::draw_mesh(glm::mat4 modelMatrix, Shader *shader) {
        // bind PBR textures
        glActiveTexture(GL_TEXTURE0);
        shader->set_int("texture_base", 0);
        glBindTexture(GL_TEXTURE_2D, pbrTexture->albedo->id);
        glActiveTexture(GL_TEXTURE1);
        shader->set_int("texture_normal", 1);
        glBindTexture(GL_TEXTURE_2D, pbrTexture->normal->id);
        glActiveTexture(GL_TEXTURE2);
        shader->set_int("texture_roughness", 2);
        glBindTexture(GL_TEXTURE_2D, pbrTexture->metalroughness->id);

        // draw
        shader->set_mat4("matrix_model", modelMatrix);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}