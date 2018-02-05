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

class SObject {
public:
    SObject() {}
    SObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl);
    glm::mat4 getGlobalTransform();
    glm::vec3 getGlobalPosition();
    void setLocalPosition(glm::vec3 position);
    void setLocalRotation(glm::vec3 ypr);
    void rotate(glm::vec3 ypr);
    glm::vec3 getGlobalScale();
    void setLocalScale(glm::vec3 scale);
    SObject* getParent();
    void setParent(SObject *parent);
    void addChild(SObject *object);
    std::vector<SObject*>& getChildren();

protected:
    SObject *parent;
    std::vector<SObject*> children;
    glm::mat4 transform;


};

class Mesh: public SObject {
public:
    Mesh(Shader *shader, Material material, std::vector<Vertex> vertices, std::vector<unsigned int> indices, GLenum edge);
    Material getMaterial();
    unsigned int getVAO();
    unsigned int getIndexCount();
    void setupMesh();
    void setTextures(Shader *pShader);
    Shader *getShader();
    void setShader(Shader *shader);

private:

    unsigned int VAO, VBO, EBO;
    Material material;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Shader *shader; //TODO use smart pointers here

    //TODO get rid of this
    GLenum edge;

};