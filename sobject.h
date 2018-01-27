#pragma once

#include <vector>
#include <memory>
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
    Texture(std::string filename, GLenum edge = GL_REPEAT);
    std::string path;
    unsigned int getId();

private:
    unsigned int id;
};

static std::vector<Texture> loaded_textures;

struct Material {
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    glm::vec3 mirrorColor;
    float exponent;
    float reflectivity;
    Texture diffuseTexture;
    Texture specularTexture;
    bool hasDiffuse;
    bool hasSpecular;
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
    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);
    Material getMaterial();
    float getScale();
    unsigned int getVAO();
    unsigned int getIndexCount();
    void setupMesh();
    void setTextures(Shader *shader);

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
    Model(char *path, Shader *shader, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, GLenum edge);
    glm::vec3 getPosition();
    void setPosition(float x, float y, float z);
    glm::vec3 getRotation();
    void setRotation(float yaw, float pitch, float roll);
    void rotate(float yaw, float pitch, float roll);
    glm::vec3 getScale();
    void setScale(glm::vec3 scale);
    std::vector<Mesh*>& getMeshes(); //TODO use smart pointers here
    Shader *getShader();
    void setShader(Shader *shader);

private:

    Shader *shader; //TODO use smart pointers here

    glm::vec3 position;
    glm::vec3 ypr;
    glm::vec3 scale;

    //TODO get rid of this
    GLenum edge;

    std::vector<Mesh*> meshes;
    std::string directory;
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh* processMesh(aiMesh* mesh, const aiScene* scene, ai_real nodeX, ai_real nodeY, ai_real nodeZ);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type);


};
