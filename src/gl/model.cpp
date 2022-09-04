#include "model.h"

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtx/transform.hpp>

namespace GLRenderer {
    void Model::init(const std::string &filePath, Shader *shader) {
        _textureManager = new TextureManager("../assets/devtex/dev_black.png");
        _modelShader = shader;

        Assimp::Importer importer;
        const aiScene *modelScene = importer.ReadFile(filePath,
                                                      aiProcess_Triangulate |
                                                      aiProcess_FlipUVs |
                                                      aiProcess_CalcTangentSpace);

        if (!modelScene || modelScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !modelScene->mRootNode) {
            std::cout << "Assimp error: " << importer.GetErrorString() << std::endl;
            return;
        }

        _directory = filePath.substr(0, filePath.find_last_of('/'));
        process_node(modelScene->mRootNode, modelScene);
    }

    void Model::set_shader(Shader *shader) {
        _modelShader = shader;
    }

    void Model::update_transform() {
        glm::mat4 newTransform = glm::mat4{1.0f};

        // translate
        newTransform = glm::translate(newTransform, glm::vec3(translation[0], translation[1], translation[2]));

        // rotate by each XYZ value
        newTransform = glm::rotate(newTransform, glm::radians(rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
        newTransform = glm::rotate(newTransform, glm::radians(rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
        newTransform = glm::rotate(newTransform, glm::radians(rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));

        // scale
        newTransform = glm::scale(newTransform, glm::vec3(scale[0], scale[1], scale[2]));

        _modelMatrix = newTransform;
    }

    void Model::draw_model(unsigned int depthTexture) {
        for (auto &mesh: meshes) {
            mesh.draw_mesh(_modelMatrix, _modelShader, depthTexture);
        }
    }

    void Model::draw_model_untextured(Shader *shader) {
        for (auto &mesh: meshes) {
            mesh.draw_mesh_untextured(_modelMatrix, shader);
        }
    }

    void Model::process_node(aiNode *node, const aiScene *scene) {
        for (size_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(process_mesh(mesh, scene));
        }
        for (size_t i = 0; i < node->mNumChildren; i++) {
            process_node(node->mChildren[i], scene);
        }
    }

    Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene) {
        Mesh newMesh;
        for (size_t i = 0; i < mesh->mNumVertices; i++) {
            Vertex newVertex{};
            newVertex.position.x = mesh->mVertices[i].x;
            newVertex.position.y = mesh->mVertices[i].y;
            newVertex.position.z = mesh->mVertices[i].z;
            if (mesh->HasNormals()) {
                newVertex.normal.x = mesh->mNormals[i].x;
                newVertex.normal.y = mesh->mNormals[i].y;
                newVertex.normal.z = mesh->mNormals[i].z;
            }
            if (mesh->mTextureCoords[0]) {
                newVertex.uv.x = mesh->mTextureCoords[0][i].x;
                newVertex.uv.y = mesh->mTextureCoords[0][i].y;
            }
            if (mesh->HasTangentsAndBitangents()) {
                newVertex.tangent.x = mesh->mTangents[i].x;
                newVertex.tangent.y = mesh->mTangents[i].y;
                newVertex.tangent.z = mesh->mTangents[i].z;
                newVertex.bitangent.x = mesh->mBitangents[i].x;
                newVertex.bitangent.y = mesh->mBitangents[i].y;
                newVertex.bitangent.z = mesh->mBitangents[i].z;
            }
            newMesh.vertices.push_back(newVertex);
        }
        for (size_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (size_t j = 0; j < face.mNumIndices; j++) {
                newMesh.indices.push_back(face.mIndices[j]);
            }
        }

        newMesh.texture = nullptr;
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture *> textureMaps;
        textureMaps.push_back(create_texture(material, aiTextureType_BASE_COLOR, "texture_base"));
        textureMaps.push_back(create_texture(material, aiTextureType_NORMALS, "texture_normal"));
        textureMaps.push_back(create_texture(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness"));

        // just use the base color path as name
        aiString str;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
        std::string fullPath = _directory + '/' + str.C_Str();
        PBRTexture *existingTexture = _textureManager->get_pbr_texture(fullPath);
        if (existingTexture) {
            newMesh.pbrTexture = existingTexture;
        } else {
            newMesh.pbrTexture = _textureManager->create_pbr_texture(textureMaps, fullPath);
        }

        newMesh.texture = textureMaps[0];

        newMesh.setup_mesh();
        return newMesh;
    }

    Texture *Model::create_texture(aiMaterial *material, aiTextureType textureType, const std::string &typeName) {
        if (!material->GetTextureCount(textureType)) {
            // no textures of this type
            return _textureManager->_defaultTexture;
        }

        aiString str;
        material->GetTexture(textureType, 0, &str);
        std::string fullPath = _directory + '/' + str.C_Str();
        // check if it already exists
        Texture *existingTexture = _textureManager->get_texture(fullPath);
        if (existingTexture) {
            return existingTexture;
        } else {
            // create new texture
            return _textureManager->create_texture(fullPath, typeName);
        }
    }

    Model *ModelManager::create_model(const std::string &filePath, const std::string &name, Shader *shader) {
        Model newModel;
        newModel.init(filePath, shader);
        models[name] = newModel;

        return &models[name];
    }
}