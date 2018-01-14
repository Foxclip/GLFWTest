#pragma once

#include <vector>
#include "stb_image.h"
#include "shader.h"

unsigned int loadTexture(char* filename, GLenum edge = GL_REPEAT, GLenum interpolation = GL_LINEAR);

class Material {
public:
    Material() {}
    Material(Shader shader, std::vector<unsigned int> textures);
    Shader getShader();
    void setTextures();

private:
    Shader shader;
    std::vector<unsigned int> textures;
};

class Cube {
public:
    Cube(float x, float y, float z, float yaw, float scale, unsigned int VBO, Material material);
    Cube(glm::vec3 pos, float yaw, float scale, unsigned int VBO, Material material);
    void render(glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos);
    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);
    Material getMaterial();

private:

    glm::vec3 position;
    float yaw;
    float scale;

    unsigned int VAO;
    Material material;

};