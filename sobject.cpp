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

Mesh::Mesh(Shader *shader, Material material, std::vector<Vertex> vertices, std::vector<unsigned int> indices, GLenum edge) {
    this->position = glm::vec3(0.0f);
    this->ypr = glm::vec3(0.0f);
    this->scale = glm::vec3(1.0f);
    this->shader = shader;
    this->material = material;
    this->vertices = vertices;
    this->indices = indices;
    this->edge = edge;
    setupMesh();
}

glm::vec3 SObject::getGlobalPosition() {
    if(parent) {
        return position + parent->getGlobalPosition();
    } else {
        return position;
    }
}

glm::vec3 SObject::getLocalPosition() {
    return position;
}

void SObject::setPosition(glm::vec3 position) {
    this->position = position;
}

glm::vec3 SObject::getGlobalRotation() {
    if(parent) {
        return ypr + parent->getGlobalRotation();
    } else {
        return ypr;
    }
}

Material Mesh::getMaterial() {
    return material;
}

glm::vec3 SObject::getLocalScale() {
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

void Mesh::setTextures(Shader *pShader) {
    pShader->use();
    pShader->setBool("material.hasDiffuse", material.hasDiffuse);
    pShader->setBool("material.hasSpecular", material.hasSpecular);
    pShader->setVec3("material.diffuseColor", material.diffuseColor);
    pShader->setVec3("material.specularColor", material.specularColor);
    pShader->setVec3("material.mirrorColor", material.mirrorColor);
    pShader->setFloat("material.shininess", material.exponent);
    pShader->setFloat("material.reflectivity", material.reflectivity);
    pShader->setInt("material.diffuse", 1);
    pShader->setInt("material.specular", 2);
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

glm::vec3 SObject::getLocalRotation() {
    return ypr;
}

void SObject::setRotation(glm::vec3 ypr) {
    this->ypr = ypr;
}

void SObject::rotate(glm::vec3 ypr) {
    this->ypr += ypr;
}

glm::vec3 SObject::getGlobalScale() {
    if(parent) {
        return scale * parent->getGlobalScale();
    } else {
        return scale;
    }
}

void SObject::setScale(glm::vec3 scale) {
    this->scale = scale;
}

SObject * SObject::getParent() {
    return parent;
}

void SObject::setParent(SObject *parent) {
    this->parent = parent;
}

void SObject::addChild(SObject *object) {
    children.push_back(object);
}

Shader *Mesh::getShader() {
    return shader;
}

void Mesh::setShader(Shader *shader) {
    this->shader = shader;
}

SObject::SObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl) {
    this->position = pos;
    this->ypr = rot;
    this->scale = scl;
}
