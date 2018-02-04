#pragma once

#include <map>
#include "sobject.h"
#include "camera.h"

struct MeshSettings {
    bool transparent = false;
    std::string directory;
    Shader *shader;
    GLenum edg;
};

class Game {
public:
    Game();
    ~Game();
    void start();
    void addDirectionalLight(float intensity, glm::vec3 color, glm::vec3 direction, glm::vec3 ambient);
    void addPointLight(float intensity, glm::vec3 color, glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient);
    void addSpotLight(float intensity, glm::vec3 color, glm::vec3 position, glm::vec3 direction, float constant, float linear, float quadratic, glm::vec3 ambient, float cutOff, float outerCutOff);
    void loadFile(char *path, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, bool transparent = false, GLenum edge = GL_REPEAT);
    void setCubeMap(std::string folder);
    void enableCubeMap();
    void setBgColor(float r, float g, float b);

    void frmbuf_size_cb(GLFWwindow* window, int width, int height);
    void mouseCb(GLFWwindow* window, double xpos, double ypos);
    void scrCb(GLFWwindow* window, double xoffset, double yoffset);

    Shader lightingShader;
    Shader zShader;
    Shader uniformShader;
    Shader pointShader;
    Shader fragShader;
    Shader normalShader;
    Shader testShader;

private:
    GLFWwindow* window;
    unsigned int screenWidth = 800;
    unsigned int screenHeight = 600;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    float lastX = 400.0f;
    float lastY = 300.0f;
    bool firstMouse = true;

    Camera camera = Camera(0.0f, 4.0f, 4.0f, 0.0f, 1.0f, 0.0f);
    //Camera camera = Camera(0.0f, 0.0f, 4.0f, 0.0f, 1.0f, 0.0f);
    //Camera camera = Camera(3.0f, 4.0f, 2.0f, 0.0f, 1.0f, 0.0f, 0.0f, -90.0f);

    unsigned int screenFrameBuffer;
    unsigned int screenColorBuffer;
    unsigned int screenVAO;
    unsigned int screenVBO;
    Shader screenShader;

    unsigned int skyboxTexture;
    unsigned int skyboxVAO;
    unsigned int skyboxVBO;
    Shader skyboxShader;

    unsigned int uboMatrices;

    std::vector<SObject*> objects;
    std::vector<Mesh*> opaqueModels;
    std::vector<Mesh*> transparentModels;
    std::vector<DirectionalLight> dirLights;
    std::vector<PointLight> pointLights;
    std::vector<SpotLight> spotLights;

    bool cubemapEnabled = false;
    float bgColorR = 0.0f;
    float bgColorG = 0.0f;
    float bgColorB = 0.0f;

    void initGLFW();
    void initFrameBuffer();
    void initUBO();
    unsigned int loadCubeMap(std::vector<std::string> faces);
    void processInput(GLFWwindow* window);
    void initShaders();
    void processPhysics();
    void renderModel(Mesh *mesh, glm::mat4 view, Shader *overrideShader = nullptr);
    void render();
    void updateLights();
    void setLights(Shader *shader, glm::mat4 viewMatrix);
    SObject* loadModel(std::string path, MeshSettings settings);
    SObject* processNode(aiNode* node, const aiScene* scene, SObject *parent, MeshSettings settings);
    Mesh* processMesh(aiMesh* mesh, const aiScene* scene, SObject *parent, MeshSettings settings);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, MeshSettings settings);

};

extern Game *game;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);