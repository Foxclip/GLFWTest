#include "game.h"
#include <functional>

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
    initCubes();
}

Game::~Game() {
    glfwTerminate();
}

void Game::addDirectionalLight(float intensity, glm::vec3 color, glm::vec3 direction, glm::vec3 ambient) {
    DirectionalLight newDirectionalLight = {intensity, color, direction, ambient};
    dirLights.push_back(newDirectionalLight);
}

void Game::addPointLight(float intensity, glm::vec3 color, glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient) {
    PointLight newPointLight = {intensity, color, position, constant, linear, quadratic, ambient};
    pointLights.push_back(newPointLight);
}

void Game::addSpotLight(float intensity, glm::vec3 color, glm::vec3 position, glm::vec3 direction, float constant, float linear, float quadratic, glm::vec3 ambient, float cutOff, float outerCutOff) {
    SpotLight newSpotLight = {intensity, color, position, direction, constant, linear, quadratic, ambient, cutOff, outerCutOff};
    spotLights.push_back(newSpotLight);
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

void Game::initCubes() {
    addDirectionalLight(1.0f, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(1.0f));
    addDirectionalLight(0.5f, glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f));

    addPointLight(5.0f, glm::vec3(1.0f, 0.5f, 0.5f), glm::vec3(-0.8f, 1.5f, 1.0f), 1.0f, 1.0f, 1.0f, glm::vec3(0.0f));
    addPointLight(10.0f, glm::vec3(1.0f, 1.0f, 0.2f), glm::vec3(-1.0f, 1.5f, -10.0f), 1.0f, 1.0f, 1.0f, glm::vec3(0.0f));

    Shader lightingShader("plain.vert", "color.frag", "Lighting");
    lightingShader.use();

    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setInt("dirLightCount", dirLights.size());
    for(int i = 0; i < dirLights.size(); i++) {
        lightingShader.setFloat("dirLights["+std::to_string(i)+"].intensity", dirLights[i].intensity);
        lightingShader.setVec3("dirLights["+std::to_string(i)+"].color", dirLights[i].color);
        lightingShader.setVec3("dirLights["+std::to_string(i)+"].ambient", dirLights[i].ambient);
    }

    lightingShader.setInt("pointLightCount", pointLights.size());
    for(int i = 0; i < pointLights.size(); i++) {
        lightingShader.setFloat("pointLights["+std::to_string(i)+"].intensity", pointLights[i].intensity);
        lightingShader.setVec3("pointLights["+std::to_string(i)+"].color", pointLights[i].color);
        lightingShader.setVec3("pointLights["+std::to_string(i)+"].position", pointLights[i].position);
        lightingShader.setFloat("pointLights["+std::to_string(i)+"].constant", pointLights[i].constant);
        lightingShader.setFloat("pointLights["+std::to_string(i)+"].linear", pointLights[i].linear);
        lightingShader.setFloat("pointLights["+std::to_string(i)+"].quadratic", pointLights[i].quadratic);
        lightingShader.setVec3("pointLights["+std::to_string(i)+"].ambient", pointLights[i].ambient);
    }

    lightingShader.setInt("spotLightCount", spotLights.size());
    for(int i = 0; i < spotLights.size(); i++) {
        lightingShader.setFloat("spotLights["+std::to_string(i)+"].intensity", spotLights[i].intensity);
        lightingShader.setVec3("spotLights["+std::to_string(i)+"].color", spotLights[i].color);
        lightingShader.setVec3("spotLights["+std::to_string(i)+"].position", spotLights[i].position);
        lightingShader.setVec3("spotLights["+std::to_string(i)+"].direction", spotLights[i].direction);
        lightingShader.setFloat("spotLights["+std::to_string(i)+"].constant", spotLights[i].constant);
        lightingShader.setFloat("spotLights["+std::to_string(i)+"].linear", spotLights[i].linear);
        lightingShader.setFloat("spotLights["+std::to_string(i)+"].quadratic", spotLights[i].quadratic);
        lightingShader.setVec3("spotLights["+std::to_string(i)+"].ambient", spotLights[i].ambient);
        lightingShader.setFloat("spotLights["+std::to_string(i)+"].cutOff", spotLights[i].cutOff);
        lightingShader.setFloat("spotLights["+std::to_string(i)+"].outerCutOff", spotLights[i].outerCutOff);
    }

    Model nanosuit("models/nanosuit/nanosuit.obj", lightingShader);
    models.push_back(nanosuit);
}

void Game::processPhysics() {
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)screenWidth/screenHeight, 0.1f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(Model model: models) {
        model.render(view, projection, dirLights, pointLights, spotLights);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Game::render() {
}

void Game::mainLoop() {
    while(!glfwWindowShouldClose(window)) {
        processInput(window);
        processPhysics();
        render();
    }
}