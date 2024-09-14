#ifndef __BYTENOL_STACK_BALL3D_SHADER_H__
#define __BYTENOL_STACK_BALL3D_SHADER_H__

#include "GLES3/gl3.h"
#include <string>
#include <iostream>


class Shader
{
    public:
        Shader() = default;
        Shader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
        void use();
        const unsigned int& getProgram() const;
        ~Shader();

    private:
        std::string infoLog;
        unsigned int program;
        bool isCompiled = true;
        bool initShader(unsigned int& shader, GLenum type, const std::string& source);
};


Shader::Shader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
    unsigned int vShader, fShader;
    isCompiled = initShader(vShader, GL_VERTEX_SHADER, vertexShaderSource) &
        initShader(fShader, GL_FRAGMENT_SHADER, fragmentShaderSource) & 1;

    program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);

    int status;
    char log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(!status) {
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << log << std::endl;
    }

    std::cout << (glIsProgram(program) & 1) << std::endl;
}


inline void Shader::use()
{
    glUseProgram(program);
}

inline const unsigned int &Shader::getProgram() const
{
    return program;
}

Shader::~Shader()
{
    glDeleteProgram(program);
}


inline bool Shader::initShader(unsigned int &shader, GLenum type, const std::string &source)
{
    const char* src = source.c_str();
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int status;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(!status) {
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << log << std::endl;
        glDeleteShader(shader);
        return false;
    }

    return true;
}

#endif 