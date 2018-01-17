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


Mesh::Mesh(float x, float y, float z, float scale, Material material, std::vector<Vertex> vertices, std::vector<unsigned int> indices) {

    position = {x, y, z};
    this->scale = scale;

    this->material = material;
    this->vertices = vertices;
    this->indices = indices;

    setupMesh();

}

Mesh::Mesh(glm::vec3 pos, float scale, Material material, std::vector<Vertex> vertices, std::vector<unsigned int> indices): Mesh(pos.x, pos.y, pos.z, scale, material, vertices, indices) {}

void Mesh::render(glm::mat4 pView, glm::mat4 pProjection, std::vector<DirectionalLight> dirLights, std::vector<PointLight> pointLights, SpotLight spotLight) {
  
    material.getShader().use();
    material.getShader().setMat4("view", pView);
    material.getShader().setMat4("projection", pProjection);
    glm::mat4 model;
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(angle), axis);
    model = glm::scale(model, glm::vec3(scale));
    material.getShader().setMat4("model", model);
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(pView * model)));
    material.getShader().setMat3("newNormal", normalMatrix);

    for(int i = 0; i < dirLights.size(); i++) {
        material.getShader().setVec3("dirLights["+std::to_string(i)+"].direction", glm::vec3(pView * glm::vec4(dirLights[i].direction, 0.0f)));
    }
    for(int i = 0; i < pointLights.size(); i++) {
        material.getShader().setVec3("pointLights["+std::to_string(i)+"].position", glm::vec3(pView * glm::vec4(pointLights[i].position, 1.0f)));
    
    material.getShader().setVec3("spotLight.direction", glm::vec3(pView * glm::vec4(spotLight.direction, 0.0f)));}
    material.getShader().setVec3("spotLight.position", glm::vec3(pView * glm::vec4(spotLight.position, 1.0f)));
    //material.getShader().setVec3("camera.position", glm::vec3(pView * glm::vec4(cameraPos, 1.0f)));

    material.setTextures();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}

glm::vec3 Mesh::getPosition() {
    return position;
}

void Mesh::setPosition(glm::vec3 position) {
    this->position = position;
}

void Mesh::setRotation(float angle, glm::vec3 axis) {
    this->angle = angle;
    this->axis = axis;
}

Material Mesh::getMaterial() {
    return material;
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

Texture::Texture(std::string slot, std::string filename) {
    this->slot = slot;
    id = loadTexture(filename);
}

unsigned int Texture::getId() {
    return id;
}
