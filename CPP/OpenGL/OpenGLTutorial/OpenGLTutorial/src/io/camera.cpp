#include "camera.h"
#include "../physics/environment.h"

/*
    constructor
*/

// default and initialize with position
Camera::Camera(glm::vec3 position)
    : cameraPos(position),
    yaw(0.0f),
    pitch(0.0f),
    speed(2.5f),
    sensitivity(1.0f),
    zoom(45.0f),
    cameraFront(glm::vec3(1.0f, 0.0f, 0.0f))
{
    updateCameraVectors();
}

/*
    modifiers
*/

// change camera direction (mouse movement)
void Camera::updateCameraDirection(double dx, double dy) {
    yaw += dx;
    pitch += dy;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    updateCameraVectors();
}

// change camera position in certain direction (keyboard)
void Camera::updateCameraPos(CameraDirection direction, double dt) {
    float velocity = (float)dt * speed;

    switch (direction) {
    case CameraDirection::FORWARD:
        cameraPos += cameraFront * velocity;
        break;
    case CameraDirection::BACKWARD:
        cameraPos -= cameraFront * velocity;
        break;
    case CameraDirection::RIGHT:
        cameraPos += cameraRight * velocity;
        break;
    case CameraDirection::LEFT:
        cameraPos -= cameraRight * velocity;
        break;
    case CameraDirection::UP:
        cameraPos += cameraUp * velocity;
        break;
    case CameraDirection::DOWN:
        cameraPos -= cameraUp * velocity;
        break;
    }
}

// change camera zoom (scroll wheel)
void Camera::updateCameraZoom(double dy) {
    if (zoom >= 1.0f && zoom <= 45.0f) {
        zoom -= dy;
    }
    else if (zoom < 1.0f) {
        zoom = 1.0f;
    }
    else { // > 45.0f
        zoom = 45.0f;
    }
}

/*
    accessors
*/

// get view matrix for camera
glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

// get zoom value for camera
float Camera::getZoom() {
    return zoom;
}

/*
    private modifier
*/

// change camera directional vectors based on movement
void Camera::updateCameraVectors() {
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

    cameraRight = glm::normalize(glm::cross(cameraFront, Environment::worldUp));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}