#ifndef __BYTENOL_STACK_BALL3D_SHADER_H__
#define __BYTENOL_STACK_BALL3D_SHADER_H__

#include "GLES3/gl3.h"
#include <string>
#include <iostream>
#include <map>
#include <memory>

class Shader;

using ShaderPtr = std::unique_ptr<Shader>;

std::map<std::string, ShaderPtr> shaders;
std::string _currentShaderName = "";

void createShader(const std::string& name, const std::string& vertexShaderSource, 
    const std::string& fragmentShaderSource, const std::vector<std::string>& locations);

void useShader(const std::string& name);

int getUniform(const std::string& name);

ShaderPtr& getShader(); 

struct Shader
{
    friend int getUniform(const std::string& name);
    friend void createShader(const std::string& name, const std::string& vertexShaderSource, 
        const std::string& fragmentShaderSource, const std::vector<std::string>& locations);

    public:
        Shader() = default;
        unsigned int& getProgram();
        ~Shader();

    private:
        unsigned int program;
        std::map<std::string, int> uniformLocations;
        void initShader(unsigned int& shader, GLenum type, const std::string& source);
};

inline unsigned int &Shader::getProgram()
{
    return program;
}

inline Shader::~Shader()
{
    glDeleteProgram(program);
}

inline void Shader::initShader(unsigned int &shader, GLenum type, const std::string &source)
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
        std::cerr << "SHADER_ERROR" << log << std::endl;
        glDeleteShader(shader);
    }
}


void createShader(const std::string& name, const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const std::vector<std::string>& locations)
{
    shaders[name] = std::unique_ptr<Shader>(new Shader());

    unsigned int vShader, fShader;
    shaders[name]->initShader(vShader, GL_VERTEX_SHADER, vertexShaderSource);
    shaders[name]->initShader(fShader, GL_FRAGMENT_SHADER, fragmentShaderSource);

    if(!glIsShader(vShader) || !glIsShader(fShader)) return;

    shaders[name]->program = glCreateProgram();
    glAttachShader(shaders[name]->program, vShader);
    glAttachShader(shaders[name]->program, fShader);
    glLinkProgram(shaders[name]->program);

    int status;
    char log[512];
    glGetProgramiv(shaders[name]->program, GL_LINK_STATUS, &status);
    if(!status) {
        glGetProgramInfoLog(shaders[name]->program, 512, nullptr, log);
        std::cerr << log << std::endl;
        return;
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    useShader(name);

    for(const auto& str: locations) {
        const char* loc = str.c_str();
        shaders[name]->uniformLocations[str] = glGetUniformLocation(shaders[name]->program, loc);
    }
}


void useShader(const std::string& name)
{
    _currentShaderName = name;
    glUseProgram(shaders[name]->getProgram());
}

int getUniform(const std::string& name)
{
    return getShader()->uniformLocations[name];
}


inline ShaderPtr &getShader()
{
    return shaders[_currentShaderName];
}

#endif 