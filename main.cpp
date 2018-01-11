#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int shaderProgram;
unsigned int VAO_1, VAO_2, VBO_1, VBO_2, EBO;

const char* vertexShaderSource = "                       \n\
    #version 330 core                                    \n\
    layout (location = 0) in vec3 aPos;                  \n\
    void main() {                                        \n\
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); \n\
    }";

const char* fragmentShaderSource = "                     \n\
    #version 330 core                                    \n\
    out vec4 FragColor;                                  \n\
    void main() {                                        \n\
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);        \n\
    }";

void compileShaders() {
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infolog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
        std::cout << "Vertex shader compilation failed\n" << infolog << "\n";
    }

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
        std::cout << "Fragment shader compilation failed\n" << infolog << "\n";
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shaderProgram, 512, NULL, infolog);
        std::cout << "Linking failed\n" << infolog << "\n";
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
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
    glUseProgram(shaderProgram);

    glBindVertexArray(VAO_1);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(VAO_2);
    glDrawArrays(GL_TRIANGLES, 0, 3);

}

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Some window", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create GLFW window" << "\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << "\n";
        return -1;
    }
   
    compileShaders();
    initTriangles();
    //initRectangle();

    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        drawTriangles();
        //drawRectangle();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}