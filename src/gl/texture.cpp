#include "texture.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <iostream>

namespace GLRenderer {
    TextureManager::TextureManager(const std::string &defaultTexturePath) {
        _defaultTexture = create_texture(defaultTexturePath, "texture_base");
    }

    Texture *TextureManager::create_texture(const std::string &filePath, const std::string &typeName) {
        int texWidth, texHeight, texChannels;

        // load the image data
        stbi_uc *pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        if (!pixels) {
            std::cout << "Failed to load texture " << filePath << ", substituting for default" << std::endl;
            return _defaultTexture;
        }

        // check texture format
        GLenum format;
        switch (texChannels) {
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                std::cout << "Unknown format for texture " << filePath << ", substituting for default" << std::endl;
                return _defaultTexture;
        }

        // create texture
        Texture newTexture;
        newTexture.type = typeName;

        // generate and set parameters
        glGenTextures(1, &newTexture.id);
        glBindTexture(GL_TEXTURE_2D, newTexture.id);
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint) format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // just use file path as texture name for now
        _textures[filePath] = newTexture;
        std::cout << "Loaded texture " << filePath << std::endl;

        stbi_image_free(pixels);
        return &_textures[filePath];
    }

    PBRTexture *TextureManager::create_pbr_texture(std::vector<Texture *> &textureMaps, const std::string &name) {
        PBRTexture newPbrTexture{};
        newPbrTexture.albedo = textureMaps[0];
        newPbrTexture.normal = textureMaps[1];
        newPbrTexture.metalroughness = textureMaps[2];

        _pbrTextures[name] = newPbrTexture;
        std::cout << "Loaded PBR texture " << name << std::endl;
        return &_pbrTextures[name];
    }

    Texture *TextureManager::get_texture(const std::string &name) {
        if (_textures.find(name) == _textures.end()) {
            // does not exist
            return nullptr;
        } else {
            return &_textures[name];
        }
    }

    PBRTexture *TextureManager::get_pbr_texture(const std::string &name) {
        if (_pbrTextures.find(name) == _pbrTextures.end()) {
            // does not exist
            return nullptr;
        } else {
            return &_pbrTextures[name];
        }
    }
}