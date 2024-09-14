#ifndef __BYTENOL_STACK_BALL3D_buffer_H__
#define __BYTENOL_STACK_BALL3D_buffer_H__

#include <memory>
#include <GLES3/gl3.h>

class BufferObject {
    public:
        BufferObject() = default;
        ~BufferObject();
        unsigned int vao;
        unsigned int vbo[2];    // vbo and ibo
};

BufferObject::~BufferObject() 
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2, vbo);
}

#endif 
