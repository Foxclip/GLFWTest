#pragma once

#include <vector>
#include "stb_image.h"
#include "shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

unsigned int loadTexture(std::string filename, GLenum edge = GL_REPEAT, GLenum interpolation = GL_LINEAR);

class Texture {
public:
    Texture() {}
    Texture(std::string slot, std::string filename);
    std::string slot;
    unsigned int getId();

private:
    unsigned int id;
};

class Material {
public:
    Material() {}
    Material(Shader shader, std::vector<Texture> textures);
    Shader getShader();
    void setTextures();

private:
    Shader shader;
    std::vector<Texture> textures;
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
    void render(glm::mat4 view, glm::mat4 projection, std::vector<DirectionalLight> dirLights, std::vector<PointLight> pointLights,  SpotLight spotLight);
    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);
    void setRotation(float angle, glm::vec3 axis);
    Material getMaterial();
    void setupMesh();

private:

    glm::vec3 position;
    float angle;
    glm::vec3 axis;
    float scale;

    unsigned int VAO, VBO, EBO;
    Material material;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

};