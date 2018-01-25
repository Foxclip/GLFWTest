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

Material::Material(Shader shader, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 mirrorColor, float exponent, float reflectivity, Texture diffuseTexture, Texture specularTexture, Texture reflectionTexture, bool hasDiffuse, bool hasSpecular) {
    this->shader = shader;
    this->diffuseColor = diffuseColor;
    this->specularColor = specularColor;
    this->mirrorColor = mirrorColor;
    this->exponent = exponent;
    this->reflectivity = reflectivity;
    this->diffuseTexture = diffuseTexture;
    this->specularTexture = specularTexture;
    this->hasDiffuse = hasDiffuse;
    this->hasSpecular = hasSpecular;
}

Shader Material::getShader() {
    return shader;
}

void Material::setTextures() {
    shader.use();
    shader.setBool("material.hasDiffuse", hasDiffuse);
    shader.setBool("material.hasSpecular", hasSpecular);
    shader.setVec3("material.diffuseColor", diffuseColor);
    shader.setVec3("material.specularColor", specularColor);
    shader.setVec3("material.mirrorColor", mirrorColor);
    shader.setFloat("material.shininess", exponent);
    shader.setFloat("material.reflectivity", reflectivity);
    shader.setInt("material.diffuse", 1);
    shader.setInt("material.specular", 2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture.getId());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, specularTexture.getId());
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

void Mesh::render(glm::mat4 model, glm::mat4 pView, glm::mat4 pProjection, std::vector<DirectionalLight> dirLights, std::vector<PointLight> pointLights, std::vector<SpotLight> spotLights) {
  
    Shader shader = material.getShader();
    shader.use();
    shader.setMat4("view", pView);
    shader.setMat4("projection", pProjection);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(scale));
    shader.setMat4("model", model);
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(pView * model)));
    glm::mat3 normalMatrixWorld = glm::mat3(glm::transpose(glm::inverse(model)));
    shader.setMat3("newNormal", normalMatrix);
    shader.setMat3("newNormalWorld", normalMatrixWorld);

    for(int i = 0; i < dirLights.size(); i++) {
        shader.setVec3("dirLights["+std::to_string(i)+"].direction", glm::vec3(pView * glm::vec4(dirLights[i].direction, 0.0f)));
    }
    for(int i = 0; i < pointLights.size(); i++) {
        shader.setVec3("pointLights[" + std::to_string(i) + "].position", glm::vec3(pView * glm::vec4(pointLights[i].position, 1.0f)));
    }
    for(int i = 0; i < spotLights.size(); i++) {
        shader.setVec3("spotLights[" + std::to_string(i) + "].direction", glm::vec3(pView * glm::vec4(spotLights[i].direction, 0.0f)));
        shader.setVec3("spotLights[" + std::to_string(i) + "].position", glm::vec3(pView * glm::vec4(spotLights[i].position, 1.0f)));
    }

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

Texture::Texture(std::string filename, GLenum edge) {
    this->path = filename;
    id = loadTexture(filename, edge);
}

unsigned int Texture::getId() {
    return id;
}

Model::Model(char *path, Shader shader, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, GLenum edge) {
    this->shader = shader;
    this->edge = edge;
    loadModel(path);
    position = pos;
    ypr = rot;
    scale = scl;
}

void Model::render(glm::mat4 view, glm::mat3 invView, glm::mat4 projection, std::vector<DirectionalLight> dirLights, std::vector<PointLight> pointLights,  std::vector<SpotLight> spotLights) {
    for(int i = 0; i < meshes.size(); i++) {
        shader.setMat3("invView", invView);
        glm::mat4 model;
        model = glm::translate(model, position);
        glm::mat4 rotation = glm::yawPitchRoll(glm::radians(ypr.x), glm::radians(ypr.y), glm::radians(ypr.z));
        model *= rotation;
        model = glm::scale(model, scale);
        meshes[i].render(model, view, projection, dirLights, pointLights, spotLights);
    }
}

void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Failed to load model " << path << "\n" << importer.GetErrorString() << "\n";
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode * node, const aiScene * scene) {
    for(int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for(int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for(int i = 0; i < mesh->mNumVertices; i++) {

        Vertex vertex;

        glm::vec3 pos;
        pos.x = mesh->mVertices[i].x;
        pos.y = mesh->mVertices[i].y;
        pos.z = mesh->mVertices[i].z;
        vertex.Position = pos;

        glm::vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        vertex.Normal = normal;

        if(mesh->mTextureCoords[0]) {
            glm::vec2 tc;
            tc.x = mesh->mTextureCoords[0][i].x;
            tc.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = tc;
        } 
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for(int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    bool hasDiffuse;
    bool hasSpecular;
    bool hasReflection;

    Texture diffuseMap;
    Texture specularMap;
    Texture reflectionMap;

    aiColor3D diffuseColor;
    aiColor3D specularColor;
    aiColor3D mirrorColor;
    ai_real exponent;
    ai_real reflectivity;

    if(mesh->mMaterialIndex >= 0) {

        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        if(material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) != aiReturn_SUCCESS) {
            diffuseColor = {0.5f, 0.5f, 0.5f};
        }
        if(material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) != aiReturn_SUCCESS) {
            specularColor = {0.5f, 0.5f, 0.5f};
        }
        if(material->Get(AI_MATKEY_COLOR_REFLECTIVE, mirrorColor) != aiReturn_SUCCESS) {
            mirrorColor = {0.0f, 0.0f, 0.0f};
        }
        if(material->Get(AI_MATKEY_SHININESS, exponent) != aiReturn_SUCCESS) {
            exponent = 100.0f;
        }
        if(material->Get(AI_MATKEY_REFLECTIVITY, reflectivity) != aiReturn_SUCCESS) {
            reflectivity = 0.0f;
        }

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
        hasDiffuse = diffuseMaps.size() > 0;
        if(hasDiffuse) {
            diffuseMap = diffuseMaps[0];
        }
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
        hasSpecular = specularMaps.size() > 0;
        if(hasSpecular) {
            specularMap = specularMaps[0];
        }

    }

    glm::vec3 diffuseColorGlmVec(diffuseColor.r, diffuseColor.g, diffuseColor.b);
    glm::vec3 specularColorGlmVec(specularColor.r, specularColor.g, specularColor.b);
    glm::vec3 mirrorColorGlmVec(mirrorColor.r, mirrorColor.g, mirrorColor.b);

    Material newMaterial(    shader,
                             diffuseColorGlmVec, specularColorGlmVec, mirrorColorGlmVec,
                             exponent, reflectivity,
                             diffuseMap, specularMap, reflectionMap,
                             hasDiffuse, hasSpecular
                        );

    return Mesh(0.0f, 0.0f, 0.0f, 1.0f, newMaterial, vertices, indices);

}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type) {
    std::vector<Texture> textures;
    for(int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString tex_path;
        mat->GetTexture(type, i, &tex_path);
        bool skip = false;
        for(int j = 0; j < loaded_textures.size(); j++) {
            if(std::strcmp(loaded_textures[j].path.c_str(), tex_path.C_Str()) == 0) {
                textures.push_back(loaded_textures[j]);
                skip = true;
                break;
            }
        }
        if(!skip) {
            Texture newTexture(directory + "/" + std::string(tex_path.C_Str()), edge);
            textures.push_back(newTexture);
            loaded_textures.push_back(newTexture);
        }
    }
    return textures;
}

glm::vec3 Model::getPosition() {
    return position;
}

void Model::setPosition(float x, float y, float z) {
    this->position = glm::vec3(x, y, z);
}

glm::vec3 Model::getRotation() {
    return ypr;
}

void Model::setRotation(float yaw, float pitch, float roll) {
    this->ypr = glm::vec3(yaw, pitch, roll);
}

void Model::rotate(float yaw, float pitch, float roll) {
    ypr.x += yaw;
    ypr.y += pitch;
    ypr.z += roll;
}

glm::vec3 Model::getScale() {
    return scale;
}

void Model::setScale(glm::vec3 scale) {
    this->scale = scale;
}
