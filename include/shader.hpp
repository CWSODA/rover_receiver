#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
   private:
    unsigned int ID;

   public:
    Shader(const char* vert_path, const char* frag_path) {
        std::string vert_code, frag_code;
        std::ifstream vert_shader_file, frag_shader_file;
        vert_shader_file.exceptions(std::ifstream::failbit |
                                    std::ifstream::badbit);
        frag_shader_file.exceptions(std::ifstream::failbit |
                                    std::ifstream::badbit);
        try {
            vert_shader_file.open(vert_path);
            frag_shader_file.open(frag_path);
            // buffer file content into streams
            std::stringstream vert_stream, frag_stream;
            vert_stream << vert_shader_file.rdbuf();
            frag_stream << frag_shader_file.rdbuf();
            // close files
            vert_shader_file.close();
            frag_shader_file.close();
            // convert streams into strings
            vert_code = vert_stream.str();
            frag_code = frag_stream.str();
        } catch (std::ifstream::failure e) {
            std::cout << "ERROR::FAILED TO READ SHADER FILE: " << std::endl;
        }
        const char* vert_shader_code = vert_code.c_str();
        const char* frag_shader_code = frag_code.c_str();

        unsigned int vert_shader = glCreateShader(GL_VERTEX_SHADER);
        unsigned int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
        int success;
        char infoLog[512];

        glShaderSource(vert_shader, 1, &vert_shader_code, NULL);
        glCompileShader(vert_shader);
        glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vert_shader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                      << "PATH: " << vert_path << "\n"
                      << infoLog << std::endl;
        }
        glShaderSource(frag_shader, 1, &frag_shader_code, NULL);
        glCompileShader(frag_shader);
        glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(frag_shader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                      << "PATH: " << frag_path << "\n"
                      << infoLog << std::endl;
        }

        ID = glCreateProgram();
        glAttachShader(ID, vert_shader);
        glAttachShader(ID, frag_shader);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << infoLog << std::endl;
        }

        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);
    }

    Shader(const char* vert_path, const char* geom_path,
           const char* frag_path) {
        std::string vert_code, frag_code, geom_code;
        std::ifstream vert_shader_file, frag_shader_file, geom_shader_file;
        vert_shader_file.exceptions(std::ifstream::failbit |
                                    std::ifstream::badbit);
        frag_shader_file.exceptions(std::ifstream::failbit |
                                    std::ifstream::badbit);
        geom_shader_file.exceptions(std::ifstream::failbit |
                                    std::ifstream::badbit);
        try {
            vert_shader_file.open(vert_path);
            frag_shader_file.open(frag_path);
            geom_shader_file.open(geom_path);
            // buffer file content into streams
            std::stringstream vert_stream, frag_stream, geom_stream;
            vert_stream << vert_shader_file.rdbuf();
            frag_stream << frag_shader_file.rdbuf();
            geom_stream << geom_shader_file.rdbuf();
            // close files
            vert_shader_file.close();
            frag_shader_file.close();
            geom_shader_file.close();
            // convert streams into strings
            vert_code = vert_stream.str();
            frag_code = frag_stream.str();
            geom_code = geom_stream.str();
        } catch (std::ifstream::failure e) {
            std::cout << "ERROR::FAILED TO READ SHADER FILE: " << std::endl;
        }
        const char* vert_shader_code = vert_code.c_str();
        const char* frag_shader_code = frag_code.c_str();
        const char* geom_shader_code = geom_code.c_str();

        unsigned int vert_shader = glCreateShader(GL_VERTEX_SHADER);
        unsigned int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
        unsigned int geom_shader = glCreateShader(GL_GEOMETRY_SHADER);
        int success;
        char infoLog[512];

        glShaderSource(vert_shader, 1, &vert_shader_code, NULL);
        glCompileShader(vert_shader);
        glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vert_shader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }
        glShaderSource(geom_shader, 1, &geom_shader_code, NULL);
        glCompileShader(geom_shader);
        glGetShaderiv(geom_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(geom_shader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }
        glShaderSource(frag_shader, 1, &frag_shader_code, NULL);
        glCompileShader(frag_shader);
        glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(frag_shader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }

        ID = glCreateProgram();
        glAttachShader(ID, vert_shader);
        glAttachShader(ID, geom_shader);
        glAttachShader(ID, frag_shader);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << infoLog << std::endl;
        }

        glDeleteShader(vert_shader);
        glDeleteShader(geom_shader);
        glDeleteShader(frag_shader);
    }

    // activate shader
    void use() { glUseProgram(ID); }

    unsigned int get_id() { return ID; }

    // change uniform variable values
    void set_bool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void set_int(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void set_float(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void set_mat4(const std::string& name, const glm::mat4& value) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                           glm::value_ptr(value));
    }

    void set_vec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void set_vec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void set_vec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void set_vec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
};