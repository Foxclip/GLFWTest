#pragma once

#include <vector>
#include "stb_image.h"
#include "shader.h"

//struct Texture {
//    unsigned int id;
//    std::string name;
//};
//
//struct Material {
//    std::vector<Texture> textures;
//    Shader shader;
//};

class Cube {
public:
    Cube(float x, float y, float z);
    void render(glm::mat4 view, glm::mat4 projection);


private:

    glm::vec3 position = {0.0f, 0.0f, 0.0f};

    unsigned int VAO;
    unsigned int VBO;
    Shader* textureShader;
    unsigned int boxTexture;
    unsigned int awTexture;


};