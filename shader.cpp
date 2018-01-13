#include "shader.h"

char* readFile(std::string filename) {
    std::fstream file(filename);
    std::string str;
    std::string contents;
    while(std::getline(file, str)) {
        contents += str;
        contents.push_back('\n');
    }
    char* new_str = (char*)malloc(sizeof(char)*strlen(contents.c_str()));
    //strcpy(new_str, contents.c_str());
    return _strdup(contents.c_str());
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
        std::string typestr;
        switch(type) {
            case GL_VERTEX_SHADER:   typestr = "vertex";   break;
            case GL_FRAGMENT_SHADER: typestr = "fragment"; break;
        }
        std::cout << name << " " << typestr << " shader compilation failed\n" << infolog << "\n";
    }
    return shader;
}

Shader::Shader(std::string vertexPath, std::string fragmentPath, char* name) {

    shaderProgram = glCreateProgram();
    int vertexShader = compileShader(vertexPath, GL_VERTEX_SHADER, name);
    int fragmentShader = compileShader(fragmentPath, GL_FRAGMENT_SHADER, name);
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
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() {
    glUseProgram(shaderProgram);
}

void Shader::setBool(const std::string & name, bool value) const {
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
}

void Shader::setInt(const std::string & name, int value) const {
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setFloat(const std::string & name, float value) const {
    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setMatrix(const std::string & name, glm::mat4 value) const {
    glad_glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}