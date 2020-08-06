#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*
    enum to represent directions for movement
*/

enum class CameraDirection {
    NONE = 0,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

/*
    camera class to help display from POV of camera
*/

class Camera {
public:
    /*
        camera values
    */

    // position
    glm::vec3 cameraPos;

    // camera directional values
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;

    // camera rotational values
    float yaw; // x-axis
    float pitch; // y-axis

    // camera movement values
    float speed;
    float sensitivity;
    float zoom;

    /*
        constructor
    */

    // default and initialize with position
    Camera(glm::vec3 position = glm::vec3(0.0f));

    /*
        modifiers
    */

    // change camera direction (mouse movement)
    void updateCameraDirection(double dx, double dy);

    // change camera position in certain direction (keyboard)
    void updateCameraPos(CameraDirection direction, double dt);

    // change camera zoom (scroll wheel)
    void updateCameraZoom(double dy);

    /*
        accessors
    */

    // get view matrix for camera
    glm::mat4 getViewMatrix();

    // get zoom value for camera
    float getZoom();

private:
    /*
        private modifier
    */

    // change camera directional vectors based on movement
    void updateCameraVectors();
};

#endif