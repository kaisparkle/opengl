#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace GLRenderer {
    struct Texture {
        unsigned int id;
        std::string type;
    };

    struct PBRTexture {
        Texture *albedo;
        Texture *normal;
        Texture *metalroughness;
    };

    class TextureManager {
    public:
        explicit TextureManager(const std::string &defaultTexturePath);

        Texture *create_texture(const std::string &filePath, const std::string &typeName);

        PBRTexture *create_pbr_texture(std::vector<Texture *> &textureMaps, const std::string &name);

        Texture *get_texture(const std::string &name);

        PBRTexture *get_pbr_texture(const std::string &name);

        Texture *_defaultTexture;

    private:
        std::unordered_map<std::string, Texture> _textures;
        std::unordered_map<std::string, PBRTexture> _pbrTextures;
    };
}