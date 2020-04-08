#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraDirection {
	NONE = 0,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera {
public:
	static Camera defaultCamera;

	glm::vec3 cameraPos;

	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;

	glm::vec3 worldUp;

	float yaw; // x-axis
	float pitch; // y-axis
	float speed;
	float sensitivity;
	float zoom;

	Camera(glm::vec3 position);

	void updateCameraDirection(double dx, double dy); // moving mouse
	void updateCameraPos(CameraDirection direction, double dt); // keyboard input
	void updateCameraZoom(double dy); // scroll wheel

	glm::mat4 getViewMatrix();
	float getZoom();

private:
	void updateCameraVectors();
};

#endif