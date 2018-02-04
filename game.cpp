#include "game.h"
#include <functional>
#include "glm/gtx/norm.hpp"

Game *game;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    game->frmbuf_size_cb(window, width, height);
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    game->mouseCb(window, xpos, ypos);

}
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    game->scrCb(window, xoffset, yoffset);
}

Game::Game() {
    initGLFW();
    initShaders();
}

Game::~Game() {
    glfwTerminate();
}

void Game::addDirectionalLight(float intensity, glm::vec3 color, glm::vec3 direction, glm::vec3 ambient) {
    DirectionalLight newDirectionalLight = {intensity, color, direction, ambient};
    dirLights.push_back(newDirectionalLight);
    lightingShader.setInt("dirLightCount", dirLights.size());
    int index = dirLights.size() - 1;
    lightingShader.use();
    lightingShader.setFloat("dirLights["+std::to_string(index)+"].intensity", dirLights[index].intensity);
    lightingShader.setVec3("dirLights["+std::to_string(index)+"].color", dirLights[index].color);
    lightingShader.setVec3("dirLights["+std::to_string(index)+"].ambient", dirLights[index].ambient);
    updateLights();
}

void Game::addPointLight(float intensity, glm::vec3 color, glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient) {
    PointLight newPointLight = {intensity, color, position, constant, linear, quadratic, ambient};
    pointLights.push_back(newPointLight);
    lightingShader.setInt("pointLightCount", pointLights.size());
    int index = pointLights.size() - 1;
    lightingShader.use();
    lightingShader.setFloat("pointLights["+std::to_string(index)+"].intensity", pointLights[index].intensity);
    lightingShader.setVec3("pointLights["+std::to_string(index)+"].color", pointLights[index].color);
    lightingShader.setVec3("pointLights["+std::to_string(index)+"].position", pointLights[index].position);
    lightingShader.setFloat("pointLights["+std::to_string(index)+"].constant", pointLights[index].constant);
    lightingShader.setFloat("pointLights["+std::to_string(index)+"].linear", pointLights[index].linear);
    lightingShader.setFloat("pointLights["+std::to_string(index)+"].quadratic", pointLights[index].quadratic);
    lightingShader.setVec3("pointLights["+std::to_string(index)+"].ambient", pointLights[index].ambient);
    updateLights();
}

void Game::addSpotLight(float intensity, glm::vec3 color, glm::vec3 position, glm::vec3 direction, float constant, float linear, float quadratic, glm::vec3 ambient, float cutOff, float outerCutOff) {
    SpotLight newSpotLight = {intensity, color, position, direction, constant, linear, quadratic, ambient, cutOff, outerCutOff};
    spotLights.push_back(newSpotLight);
    lightingShader.setInt("spotLightCount", spotLights.size());
    int index = spotLights.size();
    lightingShader.use();
    lightingShader.setFloat("spotLights["+std::to_string(index)+"].intensity", spotLights[index].intensity);
    lightingShader.setVec3("spotLights["+std::to_string(index)+"].color", spotLights[index].color);
    lightingShader.setVec3("spotLights["+std::to_string(index)+"].position", spotLights[index].position);
    lightingShader.setVec3("spotLights["+std::to_string(index)+"].direction", spotLights[index].direction);
    lightingShader.setFloat("spotLights["+std::to_string(index)+"].constant", spotLights[index].constant);
    lightingShader.setFloat("spotLights["+std::to_string(index)+"].linear", spotLights[index].linear);
    lightingShader.setFloat("spotLights["+std::to_string(index)+"].quadratic", spotLights[index].quadratic);
    lightingShader.setVec3("spotLights["+std::to_string(index)+"].ambient", spotLights[index].ambient);
    lightingShader.setFloat("spotLights["+std::to_string(index)+"].cutOff", spotLights[index].cutOff);
    lightingShader.setFloat("spotLights["+std::to_string(index)+"].outerCutOff", spotLights[index].outerCutOff);
    updateLights();
}

void Game::loadFile(char *path, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, bool transparent, GLenum edge) {
    MeshSettings settings;
    settings.directory = "";
    settings.shader = &lightingShader;
    settings.transparent = transparent;
    settings.edg = edge;
    SObject *model = loadModel(path, settings);
    model->setPosition(pos);
    model->setRotation(rot);
    model->setScale(scl);
}

void Game::frmbuf_size_cb(GLFWwindow * window, int width, int height) {
    glViewport(0, 0, width, height);
    screenWidth = width;
    screenHeight = height;
}

void Game::mouseCb(GLFWwindow * window, double xpos, double ypos) {
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    camera.processMouse(xoffset, yoffset);
}

void Game::scrCb(GLFWwindow * window, double xoffset, double yoffset) {
    camera.processScroll(yoffset);
}

void Game::initGLFW() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
    screenWidth = vidmode->width;
    screenHeight = vidmode->height;

    //true fullscreen
    //window = glfwCreateWindow(screenWidth, screenHeight, "Some window", glfwGetPrimaryMonitor(), NULL);

    //undecorated fullscreen window
    glfwWindowHint(GLFW_DECORATED, 0);
    window = glfwCreateWindow(screenWidth, screenHeight, "Some window", NULL, NULL);

    if(window == NULL) {
        std::cout << "Failed to create GLFW window" << "\n";
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << "\n";
        exit(-1);
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    initFrameBuffer();

    float skyboxVertices[] = {

        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glDepthFunc(GL_LEQUAL);

    initUBO();

    //glBindVertexArray(0);

}

void Game::initFrameBuffer() {

    //setting up framebuffers
    glGenFramebuffers(1, &screenFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);

    //setting up color buffer
    glGenTextures(1, &screenColorBuffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenColorBuffer, 0);

    //setting up render buffer
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    //if some error occured
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is not complete\n";
        exit(-1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //setting up screen quad

    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);

    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);

    float vertices[] = {

        -1.0f, -1.0f,      0.0f, 0.0f,
        -1.0f,  1.0f,      0.0f, 1.0f,
         1.0f,  1.0f,      1.0f, 1.0f,

        -1.0f, -1.0f,      0.0f, 0.0f,
         1.0f,  1.0f,      1.0f, 1.0f,
         1.0f, -1.0f,      1.0f, 0.0f

    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Game::initUBO() {

    unsigned int matrixIndexLighting = glGetUniformBlockIndex(lightingShader.getID(), "Matrices");
    unsigned int matrixIndexSkybox = glGetUniformBlockIndex(skyboxShader.getID(), "Matrices");
    glUniformBlockBinding(lightingShader.getID(), matrixIndexLighting, 0);
    glUniformBlockBinding(skyboxShader.getID(), matrixIndexSkybox, 0);

    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    unsigned int bufferSize = 3*sizeof(glm::mat4);
    glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, bufferSize);

}

unsigned int Game::loadCubeMap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    //glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannels;
    for(int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if(data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Failed to load skybox texture: " << faces[i] << "\n";
            exit(-1);
        }
        
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void Game::processInput(GLFWwindow * window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.processKeyboard(FORWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboard(BACKWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboard(LEFT, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboard(RIGHT, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.processKeyboard(UP, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.processKeyboard(DOWN, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera.movementSpeed = D_FAST_SPEED;
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
        camera.movementSpeed = D_SLOW_SPEED;
    }
}

void Game::initShaders() {
    lightingShader = Shader("plain.vert", "color.frag", "plain.geom");
    zShader = Shader("plain.vert", "z.frag");
    uniformShader = Shader("plain.vert", "uniform.frag");
    pointShader = Shader("point.vert", "uniform.frag");
    fragShader = Shader("plain.vert", "frag.frag");
    screenShader = Shader("quad.vert", "quad.frag");
    skyboxShader = Shader("skybox.vert", "skybox.frag");
    normalShader = Shader("normal.vert", "uniform.frag", "normal.geom");
    testShader = Shader("test.vert", "test.frag", "plain.geom");
}

void Game::processPhysics() {
    objects[0]->rotate(glm::vec3(1.0f, 0.0f, 0.0f));
}

void Game::renderModel(Mesh *mesh, glm::mat4 viewMatrix, Shader *overrideShader) {

    //preparing shader
    Shader *shader;
    if(overrideShader) {
        shader = overrideShader;
    } else {
        shader = mesh->getShader();
    }
    shader->use();

    glm::mat4 modelMatrix;

    //getting parent node hierarchy (bottom -> top)
    std::vector<SObject*> nodes;
    SObject *currentNode = mesh->getParent();
    while(currentNode) {
        nodes.push_back(currentNode);
        currentNode = currentNode->getParent();
    }

    //iterating over parent nodes (top -> bottom)
    for(int i = nodes.size() - 1; i >= 0; i--) {

        SObject *currentNode = nodes[i];

        //getting parent transforms
        glm::vec3 nodePosition = currentNode->getPosition();
        glm::vec3 nodeRotation = currentNode->getRotation();
        glm::vec3 nodeScale = currentNode->getScale();

        //getting parent model matrix
        modelMatrix = glm::translate(modelMatrix, nodePosition);
        glm::mat4 rotation = glm::yawPitchRoll(glm::radians(nodeRotation.x), glm::radians(nodeRotation.y), glm::radians(nodeRotation.z));
        //modelMatrix = rotation * modelMatrix;
        modelMatrix *= rotation;
        modelMatrix = glm::scale(modelMatrix, nodeScale);

        currentNode = currentNode->getParent();

    }

    //getting mesh transforms
    glm::vec3 meshPosition = mesh->getPosition();
    glm::vec3 meshRotation = mesh->getRotation();
    glm::vec3 meshScale = mesh->getScale();

    //getting mesh model matrix
    modelMatrix = glm::translate(modelMatrix, meshPosition);
    glm::mat4 rotation = glm::yawPitchRoll(glm::radians(meshRotation.x), glm::radians(meshRotation.y), glm::radians(meshRotation.z));
    modelMatrix *= rotation;
    modelMatrix = glm::scale(modelMatrix, meshScale);
    shader->setMat4("model", modelMatrix);

    //normal matrix
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(viewMatrix * modelMatrix))); //change to modelMatrix in case of problems with normals
    shader->setMat3("newNormal", normalMatrix);

    mesh->setTextures(shader);

    //finally, rendering
    shader->use();
    //uniformShader.use();
    glBindVertexArray(mesh->getVAO());
    glDrawElements(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //}
}

void Game::render() {

    //alt-tab crash prevention
    float aspectRatio;
    if(screenHeight == 0.0f) {
        aspectRatio = 1.0f;
    } else {
        aspectRatio = (float)screenWidth/screenHeight;
    }

    //initializing
    glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);
    glClearColor(bgColorR, bgColorG, bgColorB, 1.0f);
    glStencilMask(0xFF);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

    //setting matrices

    //view
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //projection
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), aspectRatio, 0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //inverted view
    glm::mat4 invView = glm::inverse(viewMatrix); //needed for reflections, because calculations are done in view space
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(invView));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //setLights(&lightingShader, viewMatrix);
    setLights(&testShader, viewMatrix);

    //render opaque models
    glEnable(GL_CULL_FACE);
    for(Mesh *model: opaqueModels) {
        renderModel(model, viewMatrix, &testShader);
    }

    //render skybox
    if(cubemapEnabled) {
        glm::mat4 skyboxView = glm::mat4(glm::mat3(viewMatrix)); //no translation
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);
        skyboxShader.use();
        skyboxShader.setMat4("skyboxView", skyboxView);
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
    }

    //sort transparent models
    std::map<float, Mesh*> sorted;
    for(Mesh *model: transparentModels) {
        float distance = glm::length2(camera.cameraPosition - model->getPosition());
        sorted[distance] = model;
    }
    //render transparent models
    glDisable(GL_CULL_FACE);
    for(std::map<float, Mesh*>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
        renderModel(it->second, viewMatrix, &testShader);
    }

    //render screen quad
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    screenShader.use();
    glBindVertexArray(screenVAO);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //final actions
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Game::updateLights() {
    lightingShader.use();
    lightingShader.setInt("dirLightCount", dirLights.size());
    lightingShader.setInt("pointLightCount", pointLights.size());
    lightingShader.setInt("spotLightCount", spotLights.size());
}

void Game::setLights(Shader *shader, glm::mat4 viewMatrix) {
    shader->use();
    for(int i = 0; i < dirLights.size(); i++) {
        shader->setVec3("dirLights[" + std::to_string(i) + "].direction", glm::vec3(viewMatrix * glm::vec4(dirLights[i].direction, 0.0f)));
    }
    for(int i = 0; i < pointLights.size(); i++) {
        shader->setVec3("pointLights[" + std::to_string(i) + "].position", glm::vec3(viewMatrix * glm::vec4(pointLights[i].position, 1.0f)));
    }
    for(int i = 0; i < spotLights.size(); i++) {
        shader->setVec3("spotLights[" + std::to_string(i) + "].direction", glm::vec3(viewMatrix * glm::vec4(spotLights[i].direction, 0.0f)));
        shader->setVec3("spotLights[" + std::to_string(i) + "].position", glm::vec3(viewMatrix * glm::vec4(spotLights[i].position, 1.0f)));
    }

    shader->setVec3("bgColor", glm::vec3(bgColorR, bgColorG, bgColorB));
    shader->setBool("skyboxEnabled", cubemapEnabled);
}

void Game::setCubeMap(std::string folder) {
    std::vector<std::string> faces = {
        folder + "/right.jpg",
        folder + "/left.jpg",
        folder + "/top.jpg",
        folder + "/bottom.jpg",
        folder + "/back.jpg",
        folder + "/front.jpg"
    };
    glDeleteTextures(1, &skyboxTexture);
    skyboxTexture = loadCubeMap(faces);
}

void Game::enableCubeMap() {
    cubemapEnabled = true;
}

void Game::setBgColor(float r, float g, float b) {
    bgColorR = r;
    bgColorG = g;
    bgColorB = b;
}

void Game::start() {
    while(!glfwWindowShouldClose(window)) {
        processInput(window);
        processPhysics();
        render();
    }
}

SObject* Game::loadModel(std::string path, MeshSettings settings) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Failed to load model " << path << "\n" << importer.GetErrorString() << "\n";
        return nullptr;
    }
    settings.directory = path.substr(0, path.find_last_of('/'));
    return processNode(scene->mRootNode, scene, nullptr, settings);
}

SObject* Game::processNode(aiNode* node, const aiScene* scene, SObject *parent, MeshSettings settings) {

    //getting node transforms
    aiMatrix4x4 nodeTransform = node->mTransformation;
    aiVector3t<float> nodeTranslation;
    aiVector3t<float> nodeRotation; //TODO convert from Euler angles to yaw-pitch-roll
    aiVector3t<float> nodeScale;
    nodeTransform.Decompose(nodeScale, nodeRotation, nodeTranslation);

    //putting transforms in SObject
    SObject *newObject = new SObject(
        glm::vec3(nodeTranslation.x, nodeTranslation.y, nodeTranslation.z),
        glm::vec3(nodeRotation.x, nodeRotation.y, nodeRotation.z),
        glm::vec3(nodeScale.x, nodeScale.y, nodeScale.z)
    );
    objects.push_back(newObject);

    //adding meshes
    for(int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *assimpMesh = scene->mMeshes[node->mMeshes[i]];
        Mesh *newMesh = processMesh(assimpMesh, scene, newObject, settings);
        newMesh->setParent(newObject);
        newObject->addChild(newMesh);
        if(settings.transparent) {
            transparentModels.push_back(newMesh);
        } else {
            opaqueModels.push_back(newMesh);
        }
    }

    //recursively calling for children
    for(int i = 0; i < node->mNumChildren; i++) {
        SObject *newNode = processNode(node->mChildren[i], scene, newObject, settings);
        newNode->setParent(newObject);
        newObject->addChild(newNode);
    }

    return newObject;

}

Mesh* Game::processMesh(aiMesh* mesh, const aiScene* scene, SObject *parent, MeshSettings settings) {

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

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, settings);
        hasDiffuse = diffuseMaps.size() > 0;
        if(hasDiffuse) {
            diffuseMap = diffuseMaps[0];
        }
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, settings);
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

    return new Mesh(&lightingShader, newMaterial, vertices, indices, settings.edg);

}

std::vector<Texture> Game::loadMaterialTextures(aiMaterial * mat, aiTextureType type, MeshSettings settings) {
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
            Texture newTexture(settings.directory + "/" + std::string(tex_path.C_Str()), settings.edg);
            textures.push_back(newTexture);
            loaded_textures.push_back(newTexture);
        }
    }
    return textures;
}