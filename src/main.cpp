#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <filesystem>

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>

#include <Shader.h>
#include <buffer.h>

glm::mat4 mProjection, mView, mModel, mIdentity;

void update(float dt);
void render();
void init(const int& w, const int& h);
void mainLoop();
std::string getTextAssets(const char* path);
std::map<std::string, std::vector<float>> objLoader(const char* path);
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
    init(W, H);
    emscripten_set_main_loop(mainLoop, 0, 1);
    exit();
    return 0;
}

float angle = 0;

void update(float dt)
{
    angle += dt;
    mModel = glm::mat4(1.0f) * glm::rotate(mIdentity, angle, glm::vec3(0.4f, 0.7f, -0.2f));
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
    bindBuffer("cube");
    useShader("basic");
    glUniformMatrix4fv(getUniform("modelMatrix"), 1, false, glm::value_ptr(mModel));
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void initShaders() {
    std::string basicVert = getTextAssets("/assets/shaders/basic.vert");
    std::string basicFrag = getTextAssets("/assets/shaders/basic.frag");

    createShader("basic", basicVert, basicFrag, {"color", "projectionMatrix", "viewMatrix", "modelMatrix"});
    glUniformMatrix4fv(getUniform("projectionMatrix"), 1, false, glm::value_ptr(mProjection));
    glUniformMatrix4fv(getUniform("viewMatrix"), 1, false, glm::value_ptr(mView));
}


void initBuffers() 
{
    float pos[]{
        // front
        -1.0, -1.0, 1.0,    1.0, 0.0, 0.0,
        1.0, -1.0, 1.0,     1.0, 0.0, 0.0,
        1.0, 1.0, 1.0,      1.0, 0.0, 0.0,
        -1.0, -1.0, 1.0,    1.0, 0.0, 0.0,
        1.0, 1.0, 1.0,      1.0, 0.0, 0.0,
        -1.0, 1.0, 1.0,     1.0, 0.0, 0.0,

        // Back face
        -1.0, -1.0, -1.0,   0.0, 1.0, 0.0,
        -1.0, 1.0, -1.0,    0.0, 1.0, 0.0,
        1.0, 1.0, -1.0,     0.0, 1.0, 0.0,
        -1.0, -1.0, -1.0,   0.0, 1.0, 0.0,
        1.0, 1.0, -1.0,     0.0, 1.0, 0.0,
        1.0, -1.0, -1.0,    0.0, 1.0, 0.0,

        // Top face
        -1.0, 1.0, -1.0,    0.0, 0.0, 1.0,
        -1.0, 1.0, 1.0,     0.0, 0.0, 1.0,
        1.0, 1.0, 1.0,      0.0, 0.0, 1.0,
        -1.0, 1.0, -1.0,    0.0, 0.0, 1.0,
        1.0, 1.0, 1.0,      0.0, 0.0, 1.0,
        1.0, 1.0, -1.0,     0.0, 0.0, 1.0,

        // Bottom face
        -1.0, -1.0, -1.0,   1.0, 1.0, 0.0,
        1.0, -1.0, -1.0,    1.0, 1.0, 0.0,
        1.0, -1.0, 1.0,     1.0, 1.0, 0.0,
        -1.0, -1.0, -1.0,   1.0, 1.0, 0.0,
        1.0, -1.0, 1.0,     1.0, 1.0, 0.0,
        -1.0, -1.0, 1.0,    1.0, 1.0, 0.0,

        // Right face
        1.0, -1.0, -1.0,    1.0, 0.0, 1.0,
        1.0, 1.0, -1.0,     1.0, 0.0, 1.0,
        1.0, 1.0, 1.0,      1.0, 0.0, 1.0,
        1.0, -1.0, -1.0,    1.0, 0.0, 1.0,
        1.0, 1.0, 1.0,      1.0, 0.0, 1.0,
        1.0, -1.0, 1.0,     1.0, 0.0, 1.0,

        // Left face
        -1.0, -1.0, -1.0,   1.0, 1.0, 1.0,
        -1.0, -1.0, 1.0,    1.0, 1.0, 1.0,
        -1.0, 1.0, 1.0,     1.0, 1.0, 1.0,
        -1.0, -1.0, -1.0,   1.0, 1.0, 1.0,
        -1.0, 1.0, 1.0,     1.0, 1.0, 1.0,
        -1.0, 1.0, -1.0,    1.0, 1.0, 1.0,
    };
    
    const auto stride = 6 * sizeof(float);
    createBuffer("cube", 1000, pos, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    unbindBuffer();
}


void init(const int& w, const int& h)
{
    float aspect = float(w) / h;

    mIdentity = glm::mat4(1.0f);
    mModel = glm::mat4(1.0f);
    //@todo reduce the far length
    mProjection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
    mView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -15.0f));

    auto ballObj = objLoader("/assets/obj/ball.obj");
    initShaders();
    initBuffers();

    glViewport(0, 0, w, h);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}


void mainLoop()
{
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


std::map<std::string, std::vector<float>> objLoader(const char* path)
{
    std::map<std::string, std::vector<float>> res;
    res["vertices"] = {};
    res["normals"] = {};

    std::ifstream objFile{ path };

    std::vector<std::vector<float>> _vertices, _normals;    // temporaries

    std::string str;
    while (getline(objFile, str))
    {
        bool shouldSplit = str.starts_with("vn") || str.starts_with("v") || str.starts_with("f");

        if(shouldSplit)
        {
            std::stringstream ss(str);
            std::string token;
            int index = 0;
            std::vector<float> data;
            while (getline(ss, token, ' '))
            {
                if(index > 0 && !str.starts_with("f"))
                    data.push_back(std::stof(token));
                else if(index > 0 && str.starts_with("f"))
                {
                    // 3//1 13//2 4//3
                    auto indexOfSlash = token.find("//");
                    int vIndex = std::stoi(token.substr(0, indexOfSlash));
                    int nIndex = std::stoi(token.substr(indexOfSlash + 2, token.size()));
                    for(int i = 0; i < 3; i++) {
                        auto& vData = _vertices[vIndex - 1][i];
                        auto& nData = _normals[nIndex - 1][i];
                        res["vertices"].push_back(vData);
                        res["normals"].push_back(nData);
                    }
                }
                index++;
            }

            if(str.starts_with("vn")) _normals.push_back(data);
            else if(str.starts_with("v")) _vertices.push_back(data);
        }
    }

    return res;
}


void exit() 
{

}