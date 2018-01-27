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

Mesh::Mesh(float x, float y, float z, float scale, Material material, std::vector<Vertex> vertices, std::vector<unsigned int> indices) {

    position = {x, y, z};
    this->scale = scale;

    this->material = material;
    this->vertices = vertices;
    this->indices = indices;

    setupMesh();

}

Mesh::Mesh(glm::vec3 pos, float scale, Material material, std::vector<Vertex> vertices, std::vector<unsigned int> indices): Mesh(pos.x, pos.y, pos.z, scale, material, vertices, indices) {}

glm::vec3 Mesh::getPosition() {
    return position;
}

void Mesh::setPosition(glm::vec3 position) {
    this->position = position;
}

Material Mesh::getMaterial() {
    return material;
}

float Mesh::getScale() {
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

Model::Model(char *path, Shader *shader, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, GLenum edge) {
    this->shader = shader;
    this->edge = edge;
    loadModel(path);
    position = pos;
    ypr = rot;
    scale = scl;
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

void Model::processNode(aiNode* node, const aiScene* scene) {
    aiMatrix4x4 nodeTransform = node->mTransformation;
    ai_real nodeX = nodeTransform.a4;
    ai_real nodeY = nodeTransform.b4;
    ai_real nodeZ = nodeTransform.c4;
    for(int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, nodeX, nodeY, nodeZ));
    }
    for(int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene, ai_real nodeX, ai_real nodeY, ai_real nodeZ) {

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

    Texture diffuseMap;
    Texture specularMap;

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

    Material newMaterial = {
                             diffuseColorGlmVec, specularColorGlmVec, mirrorColorGlmVec,
                             exponent, reflectivity,
                             diffuseMap, specularMap,
                             hasDiffuse, hasSpecular
    };
    //newMaterial.diffuseColor = diffuseColorGlmVec;
    //newMaterial

    //return std::make_unique<Mesh>(Mesh(0.0f, 0.0f, 0.0f, 1.0f, newMaterial, vertices, indices));
    return new Mesh(nodeX, nodeY, nodeZ, 1.0f, newMaterial, vertices, indices);

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

std::vector<Mesh*>& Model::getMeshes() {
    return meshes;
}

Shader *Model::getShader() {
    return shader;
}

void Model::setShader(Shader *shader) {
    this->shader = shader;
}