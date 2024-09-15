#ifndef __BYTENOL_STACK_BALL3D_CAMERA_H__
#define __BYTENOL_STACK_BALL3D_CAMERA_H__

#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <emscripten/emscripten.h>

void initCamera(int w, int h);

extern "C" EMSCRIPTEN_KEEPALIVE void setCameraPos(float x, float y, float z);
extern "C" EMSCRIPTEN_KEEPALIVE void setCameraTarget(float x, float y, float z);

struct Camera
{
    glm::mat4 perspective;
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    glm::mat4 _lookAt;

} camera;

void initCamera(int w, int h)
{
    float aspect = float(w) / h;
    camera.perspective = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
    camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
}

extern "C" EMSCRIPTEN_KEEPALIVE void setCameraPos(float x, float y, float z)
{
    camera.position.x = x;
    camera.position.y = y;
    camera.position.z = z;
}


extern "C" EMSCRIPTEN_KEEPALIVE void setCameraTarget(float x, float y, float z)
{
    camera.target.x = x;
    camera.target.y = y;
    camera.target.z = z;
}

#endif 