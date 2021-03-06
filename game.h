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
    
    SObject* loadFile(std::string path, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, bool transparent = false, GLenum edge = GL_REPEAT);
    Mesh* loadMesh(std::string path, bool transparent = false, GLenum edge = GL_REPEAT);
    void setCubeMap(std::string folder);
    void enableCubeMap();
    void setBgColor(float r, float g, float b);
    void addDirectionalLight(float intensity, glm::vec3 color, glm::vec3 direction, glm::vec3 ambient);
    void addPointLight(float intensity, glm::vec3 color, glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient);
    void addSpotLight(float intensity, glm::vec3 color, glm::vec3 position, glm::vec3 direction, float constant, float linear, float quadratic, glm::vec3 ambient, float cutOff, float outerCutOff);
    void addParticleField(std::string path, int count, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, bool transparent = false);
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

    Camera camera = Camera(glm::vec3(0.0f, 4.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //nanosuit scene
    //Camera camera = Camera(0.0f, 0.0f, 4.0f, 0.0f, 1.0f, 0.0f);
    //Camera camera = Camera(3.0f, 4.0f, 2.0f, 0.0f, 1.0f, 0.0f, 0.0f, -90.0f);
    //Camera camera = Camera(glm::vec3(-3.0f, 1.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //Camera camera = Camera(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //space scene
    //Camera camera = Camera(glm::vec3(0.0f, 2.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //parket scene

    unsigned int screenFrameBufferMultisample;
    unsigned int screenFrameBufferIntermediate;
    unsigned int screenColorBufferMultisample;
    unsigned int screenColorBufferIntermediate;
    unsigned int screenVAO;
    unsigned int screenVBO;
    Shader screenShader;

    unsigned int skyboxTexture;
    unsigned int skyboxVAO;
    unsigned int skyboxVBO;
    Shader skyboxShader;

    unsigned int uboMatrices;

    std::vector<SObject*> objects;
    std::vector<SObject*> nodes;
    std::vector<Mesh*> opaqueModels;
    std::vector<Mesh*> transparentModels;
    std::vector<ParticleField*> opaqueParticleFields;
    std::vector<DirectionalLight> dirLights;
    std::vector<PointLight> pointLights;
    std::vector<SpotLight> spotLights;

    bool cubemapEnabled = false;
    float bgColorR = 0.0f;
    float bgColorG = 0.0f;
    float bgColorB = 0.0f;

    bool testBool;

    void initGLFW();
    void initFrameBuffer();
    void initUBO();
    unsigned int loadCubeMap(std::vector<std::string> faces);
    void processInput(GLFWwindow* window);
    void initShaders();
    void processPhysics();
    void renderModel(Mesh *mesh, glm::mat4 view, Shader *overrideShader = nullptr);
    void renderParticleField(ParticleField *field, glm::mat4 view, Shader *overrideShader = nullptr);
    void render();
    void updateLights(Shader *shader);
    void setLights(Shader *shader, glm::mat4 viewMatrix);
    SObject* loadAllMeshes(std::string path, MeshSettings settings);
    Mesh* loadOneMesh(std::string path, MeshSettings settings);
    Mesh* findFirstMesh(aiNode* node, const aiScene* scene, MeshSettings settings);
    SObject* processNode(aiNode* node, const aiScene* scene, SObject *parent, MeshSettings settings);
    Mesh* processMesh(aiMesh* mesh, const aiScene* scene, MeshSettings settings);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, MeshSettings settings, bool srgb);
    void addDirectionalLight(Shader *shader, float intensity, glm::vec3 color, glm::vec3 direction, glm::vec3 ambient);
    void addPointLight(Shader *shader, float intensity, glm::vec3 color, glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient);
    void addSpotLight(Shader *shader, float intensity, glm::vec3 color, glm::vec3 position, glm::vec3 direction, float constant, float linear, float quadratic, glm::vec3 ambient, float cutOff, float outerCutOff);

};

extern Game *game;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);