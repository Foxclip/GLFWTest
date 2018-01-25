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
}

Model& Game::addModel(char *path, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, bool transparent, GLenum edge) {
    Model model(path, lightingShader, pos, rot, scl, edge);
    if(transparent) {
        transparentModels.push_back(model);
        return transparentModels.back();
    } else {
        opaqueModels.push_back(model);
        return opaqueModels.back();
    }
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
    //window = glfwCreateWindow(screenWidth, screenHeight, "Some window", glfwGetPrimaryMonitor(), NULL);
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

    initFrameBuffer();

    std::vector<std::string> faces = {
        "skybox/right.jpg",
        "skybox/left.jpg",
        "skybox/top.jpg",
        "skybox/bottom.jpg",
        "skybox/back.jpg",
        "skybox/front.jpg"
    };
    skyboxTexture = loadCubeMap(faces);

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

    //glBindVertexArray(0);

}

void Game::initFrameBuffer() {
    glGenFramebuffers(1, &screenFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);

    glGenTextures(1, &screenColorBuffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenColorBuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is not complete\n";
        exit(-1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

    //glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

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

    lightingShader = Shader("plain.vert", "color.frag", "Lighting");
    lightingShader.use();
    lightingShader.setFloat("material.shininess", 32.0f);

    zShader = Shader("plain.vert", "z.frag", "ZShader");

    uniformShader = Shader("plain.vert", "uniform.frag", "Uniform");

    screenShader = Shader("quad.vert", "quad.frag", "Screen");

    skyboxShader = Shader("skybox.vert", "skybox.frag", "Skybox");

}

void Game::processPhysics() {
    opaqueModels[0].rotate(1.0f, 0.0f, 0.0f);
}

void Game::renderModel(Model currentModel, glm::mat4 viewMatrix, glm::mat4 invViewMatrix, glm::mat4 projectionMatrix) {

    //some preparations
    glm::vec3 modelPosition = currentModel.getPosition();
    glm::vec3 modelRotation = currentModel.getRotation();
    glm::vec3 modelScale = currentModel.getScale();
    Shader currentShader = currentModel.getShader();
    currentShader.use();
    currentShader.setMat3("invView", invViewMatrix);

    //translating and rotating
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, modelPosition);
    glm::mat4 rotation = glm::yawPitchRoll(glm::radians(modelRotation.x), glm::radians(modelRotation.y), glm::radians(modelRotation.z));
    modelMatrix *= rotation;
    modelMatrix = glm::scale(modelMatrix, modelScale);

    std::vector<Mesh*>& meshes = currentModel.getMeshes();

    //rendering meshes
    for(Mesh* currentMesh: meshes) {

        //preparations
        glm::vec3 meshPosition = currentMesh->getPosition();
        float meshScale = currentMesh->getScale();
        currentShader.setMat4("view", viewMatrix);
        currentShader.setMat4("projection", projectionMatrix);

        //transforms
        glm::mat4 meshModelMatrix;
        meshModelMatrix = glm::translate(modelMatrix, meshPosition);
        meshModelMatrix = glm::scale(meshModelMatrix, glm::vec3(meshScale));
        currentShader.setMat4("model", meshModelMatrix);

        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(viewMatrix * meshModelMatrix))); //change to modelMatrix in case of problems with normals
        currentShader.setMat3("newNormal", normalMatrix);

        //TODO do this only when number of lights changes
        currentShader.setInt("dirLightCount", dirLights.size());
        currentShader.setInt("pointLightCount", pointLights.size());
        currentShader.setInt("spotLightCount", spotLights.size());

        //sending lights to shader
        for(int i = 0; i < dirLights.size(); i++) {
            currentShader.setVec3("dirLights[" + std::to_string(i) + "].direction", glm::vec3(viewMatrix * glm::vec4(dirLights[i].direction, 0.0f)));
        }
        for(int i = 0; i < pointLights.size(); i++) {
            currentShader.setVec3("pointLights[" + std::to_string(i) + "].position", glm::vec3(viewMatrix * glm::vec4(pointLights[i].position, 1.0f)));
        }
        for(int i = 0; i < spotLights.size(); i++) {
            currentShader.setVec3("spotLights[" + std::to_string(i) + "].direction", glm::vec3(viewMatrix * glm::vec4(spotLights[i].direction, 0.0f)));
            currentShader.setVec3("spotLights[" + std::to_string(i) + "].position", glm::vec3(viewMatrix * glm::vec4(spotLights[i].position, 1.0f)));
        }
        currentMesh->getMaterial().setTextures();

        //finally, rendering
        glBindVertexArray(currentMesh->getVAO());
        glDrawElements(GL_TRIANGLES, currentMesh->getIndexCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glStencilMask(0xFF);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

    //getting matrices
    glm::mat4 view = camera.getViewMatrix();
    glm::mat3 invView = glm::mat3(glm::inverse(view));
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), aspectRatio, 0.1f, 100.0f);

    //render opaque models
    glEnable(GL_CULL_FACE);
    for(Model& model : opaqueModels) {
        renderModel(model, view, invView, projection);
    }

    glm::mat4 skyboxView = glm::mat4(glm::mat3(view)); //no translation
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    skyboxShader.use();
    skyboxShader.setMat4("projection", projection);
    skyboxShader.setMat4("view", skyboxView);
    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);

    //sort transparent models
    std::map<float, Model> sorted;
    for(Model model: transparentModels) {
        float distance = glm::length2(camera.cameraPosition - model.getPosition());
        sorted[distance] = model;
    }
    //render transparent models
    glDisable(GL_CULL_FACE);
    for(std::map<float, Model>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
        renderModel(it->second, view, invView, projection);
    }

    //render screen quad
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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

void Game::start() {
    while(!glfwWindowShouldClose(window)) {
        processInput(window);
        processPhysics();
        render();
    }
}