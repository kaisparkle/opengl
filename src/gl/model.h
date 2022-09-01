#pragma once

#include <string>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <gl/texture.h>
#include <gl/mesh.h>
#include <gl/shader.h>

namespace GLRenderer {
    class Model {
    public:
        void init(const std::string &filePath, Shader *shader);

        void set_shader(Shader *newShader);

        void update_transform();

        void draw_model();

        std::vector<Mesh> meshes;

        // using public float arrays so imgui can update them
        float translation[3] = {0.0f, 0.0f, 0.0f};
        float rotation[3] = {0.0f, 0.0f, 0.0f};
        float scale[3] = {1.0f, 1.0f, 1.0f};
    private:
        Shader *_modelShader;
        glm::mat4 _modelMatrix = glm::mat4{1.0f};
        TextureManager *_textureManager;
        std::string _directory;

        void process_node(aiNode *node, const aiScene *scene);

        Mesh process_mesh(aiMesh *mesh, const aiScene *scene);

        Texture *create_texture(aiMaterial *material, aiTextureType textureType, const std::string &typeName);
    };

    class ModelManager {
    public:
        std::unordered_map<std::string, Model> models;

        Model *create_model(const std::string &filePath, const std::string &name, Shader *shader);
    };
}