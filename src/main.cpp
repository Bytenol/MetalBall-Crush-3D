/**
 * Draw a simple triangle
 * Load shaders from assets
 */
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>

#include <Shader.h>
#include <buffer.h>

void update(float dt);
void render();
void init();
void mainLoop();
std::string getTextAssets(const char* path);
void exit();


int main(int argc, char const *argv[])
{
    const char* canvasId = "#gl";
    const int W = 500;
    const int H = 640;

    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.majorVersion = 2;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context(canvasId, &attr);
    if(context <= 0) {
        std::cerr << "Unable to create webgl2 context" << std::endl;
        return -1;
    }

    emscripten_webgl_make_context_current(context);
    emscripten_set_canvas_element_size(canvasId, W, H);
    init();
    emscripten_set_main_loop(mainLoop, 0, 1);
    exit();
    return 0;
}


void update(float dt)
{
}


void render()
{
    BufferObject b;
    glGenVertexArrays(1, &b.vao);
    glGenBuffers(2, b.vbo);
    glBindVertexArray(b.vao);
    glBindBuffer(GL_ARRAY_BUFFER, *b.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(b.vbo + 1));

    float pos[]{
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
     };
    glBufferData(GL_ARRAY_BUFFER, 1000, pos, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)(0));

    useShader("basic");
    glDrawArrays(GL_TRIANGLES, 0, 4);

}

void initShaders() {
    std::string basicVert = getTextAssets("/assets/shaders/basic.vert");
    std::string basicFrag = getTextAssets("/assets/shaders/basic.frag");

    createShader("basic", basicVert, basicFrag);
}


void init()
{
    initShaders();
}


void mainLoop()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    render();
    update(1/60.0f);    // @todo use real dt for update
}


std::string getTextAssets(const char* path) 
{
    std::ifstream file{ path };
    std::string line, res;

    while (getline(file, line)) res += line + "\n";
    return res;
}


void exit() 
{

}