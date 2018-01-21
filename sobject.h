#pragma once

#include <vector>
#include "stb_image.h"
#include "shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

unsigned int loadTexture(std::string filename, GLenum edge = GL_REPEAT, GLenum interpolation = GL_LINEAR);

class Texture {
public:
    Texture() {}
    Texture(std::string typeStr, std::string filename, GLenum edge = GL_REPEAT);
    std::string typeStr;
    std::string path;
    unsigned int getId();

private:
    unsigned int id;
};

static std::vector<Texture> loaded_textures;

class Material {
public:
    Material() {}
    Material(Shader shader, std::vector<Texture> textures, bool hasDiffuse, bool hasSpecular);
    Shader getShader();
    void setTextures();

private:
    Shader shader;
    std::vector<Texture> textures;
    bool hasDiffuse = false;
    bool hasSpecular = false;
};

struct DirectionalLight {
    float intensity;
    glm::vec3 color;
    glm::vec3 direction;
    glm::vec3 ambient;
};

struct PointLight {
    float intensity;
    glm::vec3 color;
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 ambient;
};

struct SpotLight {
    float intensity;
    glm::vec3 color;
    glm::vec3 position;
    glm::vec3 direction;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 ambient;
    float cutOff;
    float outerCutOff;
};

class Mesh {
public:
    Mesh(float x, float y, float z, float scale, Material material, std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    Mesh(glm::vec3 pos, float scale, Material material, std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, std::vector<DirectionalLight> dirLights, std::vector<PointLight> pointLights, std::vector<SpotLight> spotLight);
    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);
    Material getMaterial();
    void setupMesh();

private:

    glm::vec3 position;
    float scale;

    unsigned int VAO, VBO, EBO;
    Material material;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

};

class Model {
public:
    Model() {}
    Model(char *path, Shader shader, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, GLenum edge);
    void render(glm::mat4 view, glm::mat3 invView, glm::mat4 projection, std::vector<DirectionalLight> dirLights, std::vector<PointLight> pointLights, std::vector<SpotLight> spotLights);
    glm::vec3 getPosition();
    void setPosition(float x, float y, float z);
    glm::vec3 getRotation();
    void setRotation(float yaw, float pitch, float roll);
    void rotate(float yaw, float pitch, float roll);
    glm::vec3 getScale();
    void setScale(glm::vec3 scale);

private:

    Shader shader;

    glm::vec3 position;
    glm::vec3 ypr;
    glm::vec3 scale;

    GLenum edge; //get rid of this

    std::vector<Mesh> meshes;
    std::string directory;
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);


};
