#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace GLRenderer {
    class Shader {
    public:
        unsigned int programID;

        Shader(const std::string &vertPath, const std::string &fragPath, const std::string &geomPath = "");

        void bind() const;

        void set_bool(const std::string &name, bool value) const;

        void set_int(const std::string &name, int value) const;

        void set_float(const std::string &name, float value) const;

        void set_mat4(const std::string &name, const glm::mat4 &mat) const;

        void set_glm_vec3(const std::string &name, const glm::vec3 &value) const;

        void set_float_vec3(const std::string &name, float x, float y, float z) const;

    private:
        static bool load_shader_binary(const std::string &filePath, uint32_t &id, uint32_t type);

	static bool load_shader_file(const std::string &filePath, uint32_t &id, uint32_t type);
    };
}
