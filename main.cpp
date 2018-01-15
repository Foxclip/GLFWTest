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
std::vector<DirectionalLight> dirLights;
std::vector<PointLight> pointLights;
SpotLight spotLight;

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
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    DirectionalLight dirLightLeft = {1.0f, glm::vec3(0.5f, 0.5f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f)};
    DirectionalLight dirLightRight = {0.5f, glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f)};
    dirLights.push_back(dirLightLeft);
    dirLights.push_back(dirLightRight);

    PointLight pointLight1 = {5.0f, glm::vec3(1.0f, 0.5f, 0.5f), glm::vec3(-0.8f, 1.5f, 1.0f), 1.0f, 1.0f, 1.0f, glm::vec3(0.0f)};
    PointLight pointLight2 = {10.0f, glm::vec3(1.0f, 1.0f, 0.2f), glm::vec3(-1.0f, 1.5f, -10.0f), 1.0f, 1.0f, 1.0f, glm::vec3(0.0f)};
    pointLights.push_back(pointLight1);
    pointLights.push_back(pointLight2);

    spotLight = {5.0f, glm::vec3(1.0f), camera.cameraPosition, camera.cameraFront, 1.0f, 1.0f, 1.0f, glm::vec3(0.2f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f))};

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    Texture containerDiffuse("material.diffuse", "container2.png");
    Texture containerSpecular("material.specular", "container2_specular.png");
    Texture containerEmission("material.emission", "matrix.jpg");

    Shader lightingShader("plain.vert", "color.frag", "Lighting");
    lightingShader.use();

    lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    lightingShader.setFloat("material.shininess", 8.0f);

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

    lightingShader.setFloat("spotLight.intensity", spotLight.intensity);
    lightingShader.setVec3("spotLight.color", spotLight.color);
    lightingShader.setVec3("spotLight.position", spotLight.position);
    lightingShader.setVec3("spotLight.direction", spotLight.direction);
    lightingShader.setFloat("spotLight.constant", spotLight.constant);
    lightingShader.setFloat("spotLight.linear", spotLight.linear);
    lightingShader.setFloat("spotLight.quadratic", spotLight.quadratic);
    lightingShader.setVec3("spotLight.ambient", spotLight.ambient);
    lightingShader.setFloat("spotLight.cutOff", spotLight.cutOff);
    lightingShader.setFloat("spotLight.outerCutOff", spotLight.outerCutOff);

    Material lightingMaterial(lightingShader, {containerDiffuse, containerSpecular});

    Shader lampShader("plain.vert", "lamp.frag", "Lamp");
    Material lampMaterial(lampShader, {});

    glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};

    for(glm::vec3 pos: cubePositions) {
        Cube newCube(pos, 1.0f, VBO, lightingMaterial);
        cubes.push_back(newCube);
    }

    //Cube mainCube(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 1.0f, VBO, lightingMaterial);
    //cubes.push_back(mainCube);
    //sceneMainCubeIndex = cubes.size() - 1;
    //Cube lampCube(lightPos, 0.2f, VBO, lampMaterial);
    //cubes.push_back(lampCube);
}

void processPhysics() {

    //double time = glfwGetTime();
    //double radius = 1.0;
    //cubes[sceneLightIndex].setPosition(glm::vec3(cos(time)*radius, 0.0f, sin(time)*radius));

    for(int i = 0; i < 10; i++) {
        float angle = 20.0f * i * glfwGetTime();
        cubes[i].setRotation(angle, glm::vec3(1.0f, 0.3f, 0.5f));
    }

    spotLight.position = camera.cameraPosition;
    spotLight.direction = camera.cameraFront;

}

void render() {

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)screenWidth/screenHeight, 0.1f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(Cube cube: cubes) {
        cube.render(view, projection, dirLights, pointLights, spotLight);
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