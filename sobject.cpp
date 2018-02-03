#include "sobject.h"

unsigned int loadTexture(std::string filename, GLenum edge, GLenum interpolation) {
    int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true);
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
        std::cout << "Failed to load texture " << filename << "\n";
    }
    stbi_image_free(data);
    return texture;
}

Mesh::Mesh(Shader * shader, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, Material material, std::vector<Vertex> vertices, std::vector<unsigned int> indices, GLenum edge) {
    this->shader = shader;
    this->position = pos;
    this->scale = scl;
    this->material = material;
    this->vertices = vertices;
    this->indices = indices;
    this->edge = edge;
    setupMesh();
}

glm::vec3 Mesh::getPosition() {
    return position;
}

void Mesh::setPosition(glm::vec3 position) {
    this->position = position;
}

Material Mesh::getMaterial() {
    return material;
}

glm::vec3 Mesh::getScale() {
    return scale;
}

unsigned int Mesh::getVAO() {
    return VAO;
}

unsigned int Mesh::getIndexCount() {
    return indices.size();
}

void Mesh::setupMesh() {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

}

void Mesh::setTextures(Shader *shader) {
    shader->use();
    shader->setBool("material.hasDiffuse", material.hasDiffuse);
    shader->setBool("material.hasSpecular", material.hasSpecular);
    shader->setVec3("material.diffuseColor", material.diffuseColor);
    shader->setVec3("material.specularColor", material.specularColor);
    shader->setVec3("material.mirrorColor", material.mirrorColor);
    shader->setFloat("material.shininess", material.exponent);
    shader->setFloat("material.reflectivity", material.reflectivity);
    shader->setInt("material.diffuse", 1);
    shader->setInt("material.specular", 2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material.diffuseTexture.getId());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, material.specularTexture.getId());
}

Texture::Texture(std::string filename, GLenum edge) {
    this->path = filename;
    id = loadTexture(filename, edge);
}

unsigned int Texture::getId() {
    return id;
}

glm::vec3 Mesh::getRotation() {
    return ypr;
}

void Mesh::setRotation(glm::vec3 ypr) {
    this->ypr = ypr;
}

void Mesh::rotate(glm::vec3 ypr) {
    this->ypr += ypr;
}

void Mesh::setScale(glm::vec3 scale) {
    this->scale = scale;
}

Shader *Mesh::getShader() {
    return shader;
}

void Mesh::setShader(Shader *shader) {
    this->shader = shader;
}