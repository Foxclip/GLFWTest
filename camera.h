#pragma once

#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

const float D_YAW = -90.0f;
const float D_PITCH = 0.0f;
const float D_SPEED = 2.5f;
const float D_SENSITIVITY = 0.1f;
const float D_FOV = 45.0f;

enum Direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
public:

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = D_YAW, float pitch = D_PITCH);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw = D_YAW, float pitch = D_PITCH);

    glm::vec3 position;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    float fov;

    glm::mat4 getViewMatrix();
    void processKeyboard(Direction direction, float deltaTime);
    void processMouse(float xoffset, float yoffset, bool costraintPitch = true);
    void processScroll(float yoffset);

private:

    void updateCameraVectors();

};

inline Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch): cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(D_SPEED), mouseSensitivity(D_SENSITIVITY), fov(D_FOV) {
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}

inline Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch): cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(D_SPEED), mouseSensitivity(D_SENSITIVITY), fov(D_FOV) {
    position = glm::vec3(posX, posY, posZ);
    worldUp = glm::vec3(upX, upY, upZ);
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}

inline glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + cameraFront, cameraUp);
}

inline void Camera::processKeyboard(Direction direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if(direction == FORWARD) {
        position += cameraFront * velocity;
    }
    if(direction == BACKWARD) {
        position -= cameraFront * velocity;
    }
    if(direction == LEFT) {
        position -= cameraRight * velocity;
    }
    if(direction == RIGHT) {
        position += cameraRight * velocity;
    }
}

inline void Camera::processMouse(float xoffset, float yoffset, bool costraintPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    yaw += xoffset;
    pitch += yoffset;
    if(costraintPitch) {
        if(pitch > 89.0f) {
            pitch = 89.0f;
        }
        if(pitch < -89.0f) {

        }
    }
    updateCameraVectors();
}

inline void Camera::processScroll(float yoffset) {
    if(fov >= 1.0f && fov <= 90.0f) {
        fov -= yoffset;
    }
    if(fov < 1.0f) {
        fov = 1.0f;
    }
    if(fov > 90.0f) {
        fov = 90.0f;
    }
}

inline void Camera::updateCameraVectors() {
    glm::vec3 new_front;
    new_front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    new_front.y = sin(glm::radians(pitch));
    new_front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    this->cameraFront = glm::normalize(new_front);
    cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}
