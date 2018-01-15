#pragma once

#include <vector>
#include "stb_image.h"
#include "shader.h"

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

class Cube {
public:
    Cube(float x, float y, float z, float scale, unsigned int VBO, Material material);
    Cube(glm::vec3 pos, float scale, unsigned int VBO, Material material);
    void render(glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos, glm::vec3 lightDir);
    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);
    void setRotation(float angle, glm::vec3 axis);
    Material getMaterial();

private:

    glm::vec3 position;
    float angle;
    glm::vec3 axis;
    float scale;

    unsigned int VAO;
    Material material;

};