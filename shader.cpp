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
    return _strdup(contents.c_str());
}

int compileShader(std::string filename, GLenum type) {
    unsigned int shader = glCreateShader(type);
    char* source = readFile(shaderFolderName + "/" + filename);
    if(strcmp(source, "") == 0) {
        std::cout << "Cannot open shader source: " << shaderFolderName + "/" + filename << "\n\n";
        exit(-1);
    }
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    int success;
    char infolog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, 4096, NULL, infolog);
        std::string typestr;
        switch(type) {
            case GL_VERTEX_SHADER:   typestr = "Vertex";   break;
            case GL_FRAGMENT_SHADER: typestr = "Fragment"; break;
            case GL_GEOMETRY_SHADER: typestr = "Geometry"; break;
        }
        std::cout << typestr << " shader compilation failed: " << filename << "\n" << infolog << "\n";
    }
    return shader;
}

Shader::Shader(std::string vertexPath, std::string fragmentPath, std::string geometryPath) {

    shaderProgram = glCreateProgram();
    unsigned int vertexShader = compileShader(vertexPath, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
    unsigned int geometryShader;
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    if(geometryPath != "") {
        geometryShader = compileShader(geometryPath, GL_GEOMETRY_SHADER);
        glAttachShader(shaderProgram, geometryShader);
    }
    glLinkProgram(shaderProgram);
    int success;
    char infolog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shaderProgram, 4096, NULL, infolog);
        if(geometryPath != "") {
            std::cout << "Shader linking failed: " << vertexPath << " | " << geometryPath << " | " << fragmentPath << "\n\n";
        } else {
            std::cout << "Shader linking failed: " << vertexPath << " | " << fragmentPath << "\n\n";
        }
        exit(-1);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);
}

void Shader::use() {
    glUseProgram(shaderProgram);
}

unsigned int Shader::getID() {
    return shaderProgram;
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

void Shader::setMat4(const std::string & name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat3(const std::string & name, glm::mat3 value) const {
    glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, float v1, float v2, float v3) const {
    glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), v1, v2, v3);
}

void Shader::setVec3(const std::string & name, glm::vec3 value) const {
    setVec3(name, value.x, value.y, value.z);
}

void Shader::setVec4(const std::string & name, float v1, float v2, float v3, float v4) const {
    glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), v1, v2, v3, v4);
}

void Shader::setVec4(const std::string & name, glm::vec4 value) const {
    setVec4(name, value.x, value.y, value.z, value.w);
}
