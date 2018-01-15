#include "sobject.h"

unsigned int loadTexture(std::string filename, GLenum edge, GLenum interpolation) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edge);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edge);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);
    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture\n";
    }
    stbi_image_free(data);
    return texture;
}

Material::Material(Shader shader, std::vector<Texture> textures) {

    this->shader = shader;
    this->textures = textures;

    shader.use();
    for(int i = 0; i < textures.size(); i++) {
        shader.setInt(textures[i].slot, i);
    }

}

Shader Material::getShader() {
    return shader;
}

void Material::setTextures() {
    unsigned int units[] = {
        GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4, GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7,
        GL_TEXTURE8, GL_TEXTURE9, GL_TEXTURE10, GL_TEXTURE11, GL_TEXTURE12, GL_TEXTURE13, GL_TEXTURE14, GL_TEXTURE15
    };
    shader.use();
    for(int i = 0; i < textures.size(); i++) {
        glActiveTexture(units[i]);
        glBindTexture(GL_TEXTURE_2D, textures[i].getId());
    }
}


Cube::Cube(float x, float y, float z, float yaw, float scale, unsigned int VBO, Material material) {

    position = {x, y, z};
    this->yaw = yaw;
    this->scale = scale;

    this->material = material;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

Cube::Cube(glm::vec3 pos, float yaw, float scale, unsigned int VBO, Material material): Cube(pos.x, pos.y, pos.z, yaw, scale, VBO, material) {}

void Cube::render(glm::mat4 pView, glm::mat4 pProjection, glm::vec3 lightPos) {
  
    material.getShader().use();
    material.getShader().setMat4("view", pView);
    material.getShader().setMat4("projection", pProjection);
    glm::mat4 model;
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scale));
    material.getShader().setMat4("model", model);
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(pView * model)));
    material.getShader().setMat3("newNormal", normalMatrix);

    material.getShader().setVec3("light.position", pView * glm::vec4(lightPos, 1.0));

    material.setTextures();

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    //glBindVertexArray(0);

}

glm::vec3 Cube::getPosition() {
    return position;
}

void Cube::setPosition(glm::vec3 position) {
    this->position = position;
}

Material Cube::getMaterial() {
    return material;
}

Texture::Texture(std::string slot, std::string filename) {
    this->slot = slot;
    id = loadTexture(filename);
}

unsigned int Texture::getId() {
    return id;
}
