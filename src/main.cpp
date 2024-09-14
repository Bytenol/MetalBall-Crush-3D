/**
 * Draw a simple triangle
 * Load shaders from assets
 */
#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>

#include <Shader.h>
#include <buffer.h>

std::map<std::string, Shader> shaders;

void update(float dt);
void render();
void init();
void mainLoop();


const std::string vertexShaderSource = R"(#version 300 es
layout (location=0) in vec3 position;

void main() {
    gl_Position = vec4(position, 1.0f);
})";

const std::string fragmentShaderSource = R"(#version 300 es
precision highp float;

out vec4 FragColor;

void main() {
    FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
})";

unsigned int program;

void initShader(unsigned int &shader, GLenum type, const std::string &source)
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
        return;
    }
}


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
    
    return 0;
}

void update(float dt)
{
}

void render()
{
    // shader = &shaders["basic"];
    // shader->use();
    
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

    glUseProgram(program);
    glDrawArrays(GL_TRIANGLES, 0, 4);

}

void init()
{
    unsigned vShader, fShader;
    initShader(vShader, GL_VERTEX_SHADER, vertexShaderSource);
    initShader(fShader, GL_FRAGMENT_SHADER, fragmentShaderSource);

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

    std::cout << program << std::endl;
}

void mainLoop()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    render();
    update(1/60.0f);    // @todo use real dt for update
}