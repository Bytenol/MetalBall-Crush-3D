#ifndef __BYTENOL_STACK_BALL3D_buffer_H__
#define __BYTENOL_STACK_BALL3D_buffer_H__

#include <memory>
#include <map>
#include <string>
#include <GLES3/gl3.h>

class BufferObject;

using BufferObjectPtr = std::unique_ptr<BufferObject>;

std::string _currentBuffer;
std::map<std::string, BufferObjectPtr> _buffers;

void createBuffer(const std::string& name, GLsizeiptr size, const void* data, GLenum usage);
void bindBuffer(const std::string& name);
BufferObjectPtr& getBuffer();
void unbindBuffer();

class BufferObject {
    friend void createBuffer(const std::string& name, GLsizeiptr size, const void* data, GLenum usage);

    public:
        BufferObject() = default;
        unsigned int& getVAO();
        unsigned int& getVBO();
        unsigned int& getIBO();
        ~BufferObject();

    private:
        unsigned int vao;
        unsigned int vbo[2];    // vbo and ibo
};

inline unsigned int &BufferObject::getVAO()
{
    return vao;
}

inline unsigned int &BufferObject::getVBO()
{
    return *vbo;
}

inline unsigned int &BufferObject::getIBO()
{
    return *(vbo + 1);
}

BufferObject::~BufferObject()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2, vbo);
}


void createBuffer(const std::string& name, GLsizeiptr size, const void* data, GLenum usage)
{
    _buffers[name] = std::unique_ptr<BufferObject>(new BufferObject());
    glGenVertexArrays(1, &_buffers[name]->vao);
    glGenBuffers(2, _buffers[name]->vbo);

    glBindVertexArray(_buffers[name]->vao);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[name]->getVBO());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[name]->getIBO());

    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    _currentBuffer = name;
}


void bindBuffer(const std::string& name)
{
    _currentBuffer = name;
    glBindVertexArray(getBuffer()->getVAO());
}


BufferObjectPtr& getBuffer()
{
    return _buffers[_currentBuffer];
}


void unbindBuffer()
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

#endif 
