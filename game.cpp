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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

}

void Game::processPhysics() {
    opaqueModels[0].rotate(1.0f, 0.0f, 0.0f);
}

void Game::render() {
    glm::mat4 view = camera.getViewMatrix();
    float aspectRatio;
    if(screenHeight == 0.0f) {
        aspectRatio = 1.0f;
    } else {
        aspectRatio = (float)screenWidth/screenHeight;
    }
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), aspectRatio, 0.1f, 100.0f);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glStencilMask(0xFF);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    std::map<float, Model> sorted;
    for(Model model: transparentModels) {
        float distance = glm::length2(camera.cameraPosition - model.getPosition());
        sorted[distance] = model;
    }

    glEnable(GL_CULL_FACE);
    for(Model model: opaqueModels) {
        model.render(view, projection, dirLights, pointLights, spotLights);
    }
    glDisable(GL_CULL_FACE);
    for(std::map<float, Model>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
        it->second.render(view, projection, dirLights, pointLights, spotLights);
    }

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