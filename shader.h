#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    Shader(const std::string& vertex_path, const std::string& fragment_path) {
        // load shaders
        // ------------
        std::string vertex_code, fragment_code;
        std::ifstream vertex_shader_file, fragment_shader_file;
        
        // ensure ifstream objects can throw exceptions
        vertex_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragment_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            // open files
            vertex_shader_file.open(vertex_path);
            fragment_shader_file.open(fragment_path);
            std::stringstream vertex_shader_stream, fragment_shader_stream;

            // read the file's buffer contents into streams
            vertex_shader_stream << vertex_shader_file.rdbuf();
            fragment_shader_stream << fragment_shader_file.rdbuf();

            // close file handlers
            vertex_shader_file.close();
            fragment_shader_file.close();

            // convert stream into string
            vertex_code = vertex_shader_stream.str();
            fragment_code = fragment_shader_stream.str();
        } catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        }

        const char* vertex_shader_code = vertex_code.c_str();
        const char* fragment_shader_code = fragment_code.c_str();

        // compile shaders
        // ---------------
        int  success;
        char info_log[512];

        // vertex shader
        unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_code, nullptr);
        glCompileShader(vertex_shader);
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
            std::cout << "ERROR:SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
        }

        // fragment shader
        unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_code, nullptr);
        glCompileShader(fragment_shader);
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
            std::cout << "ERROR:SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
        }

        // shader program
        this->shader_program = glCreateProgram();
        glAttachShader(this->shader_program, vertex_shader);
        glAttachShader(this->shader_program, fragment_shader);
        glLinkProgram(this->shader_program);
        glGetProgramiv(this->shader_program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(this->shader_program, 512, nullptr, info_log);
            std::cout << "ERROR:SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
        }

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    void use() { glUseProgram(this->shader_program); }

    void set_bool(const std::string &name, bool value) const {         
        glUniform1i(glGetUniformLocation(this->shader_program, name.c_str()), (int)value); 
    }


    void set_int(const std::string &name, int value) const { 
        glUniform1i(glGetUniformLocation(this->shader_program, name.c_str()), value); 
    }


    void set_float(const std::string &name, float value) const { 
        glUniform1f(glGetUniformLocation(this->shader_program, name.c_str()), value); 
    }

    void set_vec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(this->shader_program, name.c_str()), 1, glm::value_ptr(value));
    }

    void set_vec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(this->shader_program, name.c_str()), x, y);
    }

    void set_vec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(this->shader_program, name.c_str()), 1, glm::value_ptr(value));
    }

    void set_vec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(this->shader_program, name.c_str()), x, y, z);
    }

    void set_vec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(this->shader_program, name.c_str()), 1, glm::value_ptr(value));
    }

    void set_vec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(this->shader_program, name.c_str()), x, y, z, w);
    }

    void set_mat2(const std::string& name, const glm::mat2& value) const {
        glUniformMatrix4fv(glGetUniformLocation(this->shader_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void set_mat3(const std::string& name, const glm::mat3& value) const {
        glUniformMatrix3fv(glGetUniformLocation(this->shader_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void set_mat4(const std::string& name, const glm::mat4& value) const {
        glUniformMatrix4fv(glGetUniformLocation(this->shader_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

private:
    unsigned int shader_program;
};

#endif 