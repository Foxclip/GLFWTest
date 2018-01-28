#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

const std::string shaderFolderName = "shaders";

class Shader {
public:
    Shader() {}
    Shader(std::string vertexPath, std::string fragmentPath, char* name);
    void use();
    unsigned int getID();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
    void setMat3(const std::string &name, glm::mat3 value) const;
    void setVec3(const std::string &name, float v1, float v2, float v3) const;
    void setVec3(const std::string &name, glm::vec3 value) const;
    void setVec4(const std::string &name, float v1, float v2, float v3, float v4) const;
    void setVec4(const std::string &name, glm::vec4 value) const;

private:
    unsigned int shaderProgram;

};