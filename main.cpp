#include <iostream>
#include <fstream>
#include <string>
#include "camera.h"
#include "sobject.h"

GLFWwindow* window;
unsigned int screenWidth = 800;
unsigned int screenHeight = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;

Camera camera(0.0f, 0.0f, 4.0f, 0.0f, 1.0f, 0.0f);

std::vector<Cube> cubes;
unsigned int sceneLightIndex;
unsigned int sceneMainCubeIndex;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    screenWidth = width;
    screenHeight = height;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
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

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processScroll(yoffset);
}

void initGLFW() {
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

void processInput(GLFWwindow* window) {

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

}

void initCubes() {

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    glm::vec3 lightPos(-0.8f, 1.5f, 1.0f);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    Shader lightingShader("plain.vert", "color.frag", "Lighting");
    lightingShader.use();
    lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("material.specular", 1.0f, 0.5f, 0.31f);
    lightingShader.setFloat("material.shininess", 32.0f);
    lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    Material lightingMaterial(lightingShader, {});

    Shader emeraldShader("plain.vert", "color.frag", "Emerald");
    emeraldShader.use();
    emeraldShader.setVec3("material.ambient", 0.0215f, 0.1745f, 0.0215f);
    emeraldShader.setVec3("material.diffuse", 0.07568f, 0.61424f, 0.07568f);
    emeraldShader.setVec3("material.specular", 0.633f, 0.727811f, 0.633f);
    emeraldShader.setFloat("material.shininess", 76.8f);
    emeraldShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    emeraldShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    emeraldShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    Material emeraldMaterial(emeraldShader, {});

    Shader lampShader("plain.vert", "lamp.frag", "Lamp");
    Material lampMaterial(lampShader, {});

    Cube mainCube(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 1.0f, VBO, lightingMaterial);
    cubes.push_back(mainCube);
    sceneMainCubeIndex = cubes.size() - 1;
    Cube lampCube(lightPos, 0.0f, 0.2f, VBO, lampMaterial);
    cubes.push_back(lampCube);
    sceneLightIndex = cubes.size() - 1;
    Cube emeraldCube(glm::vec3(2.0f, 0.0f, 0.0f), 0.0f, 1.0f, VBO, emeraldMaterial);
    cubes.push_back(emeraldCube);
}

void processPhysics() {

    double time = glfwGetTime();
    double radius = 1.0;
    cubes[sceneLightIndex].setPosition(glm::vec3(cos(time)*radius, 0.0f, sin(time)*radius));

    glm::vec3 lightColor;
    lightColor.x = sin(glfwGetTime() * 2.0f)/2.0f + 0.5f;
    lightColor.y = sin(glfwGetTime() * 0.7f)/2.0f + 0.5f;
    lightColor.z = sin(glfwGetTime() * 1.3f)/2.0f + 0.5f;
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
    Shader cubeMatShader = cubes[sceneMainCubeIndex].getMaterial().getShader();
    cubeMatShader.use();
    cubeMatShader.setVec3("light.ambient", ambientColor);
    cubeMatShader.setVec3("light.diffuse", diffuseColor);

}

void render() {

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)screenWidth/screenHeight, 0.1f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(Cube cube: cubes) {
        cube.render(view, projection, cubes[sceneLightIndex].getPosition());
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void mainLoop() {
    while(!glfwWindowShouldClose(window)) {
        processInput(window);
        processPhysics();
        render();
    }
}

int main() {

    initGLFW();
    initCubes();

    mainLoop();

    glfwTerminate();

    return 0;

}