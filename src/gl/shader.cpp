#include "shader.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

namespace GLRenderer {
    Shader::Shader(const std::string &vertPath, const std::string &fragPath, const std::string &geomPath) {
        // only support loading from SPIR-V for now
        uint32_t vertexShader = 0;
        uint32_t fragShader = 0;
        uint32_t geomShader = 0;
        if (!load_shader_binary(vertPath, vertexShader, GL_VERTEX_SHADER)) {
            std::cout << "Failed to load vertex shader" << std::endl;
        }
        if (!load_shader_binary(fragPath, fragShader, GL_FRAGMENT_SHADER)) {
            std::cout << "Failed to load fragment shader" << std::endl;
        }
        if (!geomPath.empty()) {
            if (!load_shader_binary(geomPath, geomShader, GL_GEOMETRY_SHADER)) {
                std::cout << "Failed to load geometry shader" << std::endl;
            }
        }

        // link the shaders
        programID = glCreateProgram();
        glAttachShader(programID, vertexShader);
        glAttachShader(programID, fragShader);
        if (!geomPath.empty()) {
            glAttachShader(programID, geomShader);
        }
        glLinkProgram(programID);

        // check
        int result = 0;
        glGetProgramiv(programID, GL_LINK_STATUS, &result);
        if (!result) {
            char infoLog[512];
            glGetProgramInfoLog(programID, 512, nullptr, infoLog);
            std::cout << "Shader linking failed\n" << infoLog << std::endl;
        }

        // delete once final program is linked
        glDeleteShader(vertexShader);
        glDeleteShader(fragShader);
        if (!geomPath.empty()) {
            glDeleteShader(geomShader);
        }
    }

    void Shader::bind() const {
        glUseProgram(programID);
    }

    void Shader::set_bool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(programID, name.c_str()), (int) value);
    }

    void Shader::set_int(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
    }

    void Shader::set_float(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
    }

    void Shader::set_mat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_glm_vec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
    }

    void Shader::set_float_vec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
    }

    bool Shader::load_shader_binary(const std::string &filePath, uint32_t &id, uint32_t type) {
        // open file, cursor at end
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            return false;
        }

        // get file size by checking location of cursor
        size_t fileSize = (size_t) file.tellg();
        // glShaderBinary expects char buffer
        std::vector<unsigned char> buffer(fileSize);
        // return cursor to beginning
        file.seekg(0);
        // read entire file into buffer
        file.read((char *) buffer.data(), (std::streamsize) fileSize);
        // done with file, clean up
        file.close();

        // create the GL shader
        id = glCreateShader(type);
        glShaderBinary(1, &id, GL_SHADER_BINARY_FORMAT_SPIR_V, buffer.data(), (GLsizei) buffer.size());
        glSpecializeShader(id, "main", 0, nullptr, nullptr);

        // check
        int result = 0;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (!result) {
            char infoLog[512];
            glGetShaderInfoLog(id, 512, nullptr, infoLog);
            std::cout << "Shader loading failed\n" << infoLog << std::endl;
        }
        return result;
    }

    bool Shader::load_shader_file(const std::string &filePath, uint32_t &id, uint32_t type) {
        // load shader source from file
        std::ifstream shaderFile;
        shaderFile.open(filePath);
        if(!shaderFile) {
            std::cout << "Failed to load shader source" << std::endl;
            return false;
        }

        // read in shader source
        std::string shaderCode;
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();

        // convert to char for compiling
        shaderCode = shaderStream.str();
        const char* shaderCodeChar = shaderCode.c_str();

        // compile
        id = glCreateShader(type);
        glShaderSource(id, 1, &shaderCodeChar, nullptr);
        glCompileShader(id);

        // check
        int result = 0;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (!result) {
            char infoLog[512];
            glGetShaderInfoLog(id, 512, nullptr, infoLog);
            std::cout << "Shader loading failed\n" << infoLog << std::endl;
        }

        return result;
    }
}
