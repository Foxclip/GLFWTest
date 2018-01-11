#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* window;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int shaderProgramOrange, shaderProgramYellow, shaderProgramRed;
unsigned int VAO_1, VAO_2, VBO_1, VBO_2, EBO;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Some window", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create GLFW window" << "\n";
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << "\n";
        exit(-1);
    }
}

char* readFile(std::string filename) {
    std::fstream file(filename);
    std::string str;
    std::string contents;
    while(std::getline(file, str)) {
        contents += str;
        contents.push_back('\n');
    }
    char* new_str = (char*)malloc(sizeof(char)*strlen(contents.c_str()));
    strcpy(new_str, contents.c_str());
    return new_str;
}

int compileShader(std::string filename, GLenum type, char* name) {
    int shader = glCreateShader(type);
    char* source = readFile(filename);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    int success;
    char infolog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, 512, NULL, infolog);
        char* typestr;
        switch(type) {
            case GL_VERTEX_SHADER:   typestr = "vertex";   break;
            case GL_FRAGMENT_SHADER: typestr = "fragment"; break;
        }
        std::cout << name << " " << typestr << " shader compilation failed\n" << infolog << "\n";
    }
    return shader;
}

int createShaderProgram(int vertexShader, int fragmentShader, char* name) {
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    int success;
    char infolog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shaderProgram, 512, NULL, infolog);
        std::cout << name << " linking failed\n" << infolog << "\n";
    }
    return shaderProgram;
}

void compileShaders() {

    int vertexShader = compileShader("simple.vert", GL_VERTEX_SHADER, "Simple");
    int orangeFragmentShader = compileShader("orange.frag", GL_FRAGMENT_SHADER, "Orange");
    int yellowFragmentShader = compileShader("yellow.frag", GL_FRAGMENT_SHADER, "Yellow");
    int redVertexShader = compileShader("red.vert", GL_VERTEX_SHADER, "Red");
    int forwardFragmentShader = compileShader("forward.frag", GL_FRAGMENT_SHADER, "Forward");

    shaderProgramOrange = createShaderProgram(vertexShader, orangeFragmentShader, "Orange");
    shaderProgramYellow = createShaderProgram(vertexShader, yellowFragmentShader, "Yellow");
    shaderProgramRed = createShaderProgram(redVertexShader, forwardFragmentShader, "Red");

    glDeleteShader(vertexShader);
    glDeleteShader(orangeFragmentShader);
    glDeleteShader(yellowFragmentShader);

}

void initTriangles() {

    float vertices1[] = {
        -1.0f,  0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
    };

    float vertices2[] = {
         0.5f,  0.5f, 0.0f,
         1.0f, -0.5f, 0.0f,
         0.0f, -0.5f, 0.0f
    };

    glGenVertexArrays(1, &VAO_1);
    glGenBuffers(1, &VBO_1);

    glBindVertexArray(VAO_1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &VAO_2);
    glGenBuffers(1, &VBO_2);

    glBindVertexArray(VAO_2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void drawTriangles() {

    glUseProgram(shaderProgramRed);
    glBindVertexArray(VAO_1);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(shaderProgramYellow);
    glBindVertexArray(VAO_2);
    glDrawArrays(GL_TRIANGLES, 0, 3);

}

void processInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void mainLoop() {
    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        drawTriangles();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main() {

    initGLFW();

    compileShaders();
    initTriangles();

    mainLoop();

    glfwTerminate();
    return 0;

}