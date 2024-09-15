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
#include <Camera.h>

int W = 500;
int H = 640;
glm::mat4 mModel;

void update(float dt);
void render();
void init(const int& w, const int& h);
void mainLoop();
std::string getTextAssets(const char* path);
std::map<std::string, std::vector<float>> objLoader(const char* path);
void exit();

extern "C" void EMSCRIPTEN_KEEPALIVE setWindowSize(int w, int h)
{
    W = w;
    H = h;
    emscripten_set_canvas_element_size("#gl", w, h);
}


int main(int argc, char const *argv[])
{
    const char* canvasId = "#gl";

    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.majorVersion = 2;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context(canvasId, &attr);
    if(context <= 0) {
        std::cerr << "Unable to create webgl2 context" << std::endl;
        return -1;
    }

    emscripten_webgl_make_context_current(context);
    init(W, H);
    emscripten_set_main_loop(mainLoop, 0, 1);
    exit();
    return 0;
}


void update(float dt)
{
    camera._lookAt = glm::lookAt(camera.position, camera.target, camera.up);
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
    // bindBuffer("cube");
    // useShader("basic");
    // glUniformMatrix4fv(getUniform("modelMatrix"), 1, false, glm::value_ptr(mModel));
    // glDrawArrays(GL_TRIANGLES, 0, 36);

    useShader("basic");
    glUniformMatrix4fv(getUniform("viewMatrix"), 1, false, glm::value_ptr(camera._lookAt));

    bindBuffer("metalBall");
    mModel = glm::mat4(1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.0f));
    // * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
    glUniformMatrix4fv(getUniform("modelMatrix"), 1, false, glm::value_ptr(mModel));
    glDrawArrays(GL_TRIANGLES, 0, getBuffer()->getSize() / 4 / 9);

    bindBuffer("staticBuffer");
    mModel = glm::mat4(1.0f);
    glUniformMatrix4fv(getUniform("modelMatrix"), 1, false, glm::value_ptr(mModel));
    glDrawArrays(GL_TRIANGLES, 0, getBuffer()->getSize() / 4 / 9);
}

void initShaders() {
    std::string basicVert = getTextAssets("/assets/shaders/basic.vert");
    std::string basicFrag = getTextAssets("/assets/shaders/basic.frag");

    createShader("basic", basicVert, basicFrag, {"color", "projectionMatrix", "viewMatrix", "modelMatrix", "directionalLightPos"});
    glUniformMatrix4fv(getUniform("projectionMatrix"), 1, false, glm::value_ptr(camera.perspective));
    glUniform3f(getUniform("directionalLightPos"), 0.0f, 1.0f, 5.0f);
}


void initBuffers() 
{
    GLsizeiptr bufferSize;
    std::vector<float> bufferData;

    // std::cout << "The size is " << (getBuffer()->getSize() / 4 / 6) << std::endl;

    // for metal ball
    auto ballObj = objLoader("/assets/obj/ball.obj");
    const int ballObjMaxSize = ballObj["vertices"].size();
    bufferData = ballObj["vertices"];
    bufferData.insert(bufferData.end(), ballObj["normals"].begin(), ballObj["normals"].end());

    for(int i = 0; i < ballObj["vertices"].size() / 3; i++) 
    {
        bufferData.push_back(1.0f);
        bufferData.push_back(0.0f);
        bufferData.push_back(0.0f);
    }

    createBuffer("metalBall", bufferData.size() * sizeof(float), bufferData.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(ballObjMaxSize * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(ballObjMaxSize * 2 * sizeof(float)));
    unbindBuffer();

    // obstacle test
    bufferData.clear();

    unsigned int step = 1;
    float radius = 1.0f;
    for(int i = 0; i < 360; i += step)
    {
        float a1 = glm::radians(static_cast<float>(i));
        float a2 = glm::radians(static_cast<float>(i + step));

        float py = 0.0f;

        float px = glm::sin(a1) * radius;
        float pz = glm::cos(a1) * radius;

        float px2 = glm::sin(a2) * radius;
        float pz2 = glm::cos(a2) * radius;

        // front data
        std::vector<float> data = {
            px, py, pz,                 1.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
            px2, py, pz,                1.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
            px2, py - radius, pz,       1.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,

            // px, py, pz,                 1.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
            // px2, py, pz,                1.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
            // px2, py + radius, pz,       1.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
        };

        // bufferData.insert(bufferData.end(), data.begin(), data.end());
    }

    // front data
    std::vector<float> data = {
        -1.0, -1.0, 1.0,                 1.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
        1.0, -1.0, 1.0,                1.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
        1.0, 1.0, 1.0,       1.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
    };

    createBuffer("staticBuffer", bufferData.size() * sizeof(float), bufferData.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    unbindBuffer();

}


void init(const int& w, const int& h)
{
    initCamera(w, h);
    initShaders();
    initBuffers();

    camera.position.y = 0;
    camera.position.z = 5;

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