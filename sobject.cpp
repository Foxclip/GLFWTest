#include "sobject.h"
#include "glm/gtx/matrix_decompose.hpp"

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
    this->transform = glm::mat4();
    this->shader = shader;
    this->material = material;
    this->vertices = vertices;
    this->indices = indices;
    this->edge = edge;
    setupMesh();

    this->parent = nullptr;

}

glm::mat4 SObject::getGlobalTransform() {
    if(parent) {
        return parent->getGlobalTransform() * transform;
    } else {
        return transform;
    }
}

glm::vec3 SObject::getGlobalPosition() {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(getGlobalTransform(), scale, rotation, translation, skew, perspective);
    return translation;
}

void SObject::setLocalPosition(glm::vec3 position) {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);
    glm::vec3 delta = position - translation;
    transform = glm::translate(transform, delta);
}

void SObject::setLocalRotation(glm::vec3 ypr) {
    glm::vec3 newYpr = glm::vec3(glm::radians(ypr.x), glm::radians(ypr.y), glm::radians(ypr.z));
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);
    glm::vec3 currentYpr(glm::yaw(rotation), glm::pitch(rotation), glm::roll(rotation));
    glm::vec3 delta = newYpr - currentYpr;
    glm::mat4 rot = glm::yawPitchRoll(delta.x, delta.y, delta.z);
    transform *= rot;
}

Material Mesh::getMaterial() {
    return material;
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

void SObject::rotate(glm::vec3 ypr) {
    glm::vec3 deltaYpr = glm::vec3(glm::radians(ypr.x), glm::radians(ypr.y), glm::radians(ypr.z));
    glm::mat4 rotation = glm::yawPitchRoll(deltaYpr.x, deltaYpr.y, deltaYpr.z);
    transform *= rotation;
}

glm::vec3 SObject::getGlobalScale() {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(getGlobalTransform(), scale, rotation, translation, skew, perspective);
    return scale;
}

void SObject::setLocalScale(glm::vec3 newScale) {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);
    glm::vec3 delta = newScale / scale;
    transform = glm::scale(transform, delta);
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

std::vector<SObject*>& SObject::getChildren() {
    return children;
}

Shader *Mesh::getShader() {
    return shader;
}

void Mesh::setShader(Shader *shader) {
    this->shader = shader;
}

SObject::SObject() {
    parent = nullptr;
}

SObject::SObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl) {
    glm::mat4 matrix;
    matrix = glm::translate(matrix, pos);
    glm::vec3 deltaYpr = glm::vec3(glm::radians(rot.x), glm::radians(rot.y), glm::radians(rot.z));
    glm::mat4 rotation = glm::yawPitchRoll(deltaYpr.x, deltaYpr.y, deltaYpr.z);
    matrix *= rotation;
    matrix = glm::scale(matrix, scl);
    transform = matrix;
}

ParticleField::ParticleField(Mesh *mesh, int count) {
    modelMatrices.resize(count);
    this->mesh = mesh;
    this->count = count;
    updateModelMatrices();
}

Mesh* ParticleField::getMesh() {
    return mesh;
}

std::vector<glm::mat4>& ParticleField::getModelMatrices() {
    return modelMatrices;
}

void ParticleField::updateModelMatrices() {

    srand(glfwGetTime()); // initialize random seed	
    float radius = 10.0;
    float offset = 2.5f;

    for(unsigned int i = 0; i < count; i++) {

        glm::mat4 model;

        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)count * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = 1.0f / 100.0f;
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;

    }
}
