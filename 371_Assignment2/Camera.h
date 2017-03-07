#pragma once

#include <vector>
//#include <iostream>

#define GLEW_STATIC
#include "GL\glew.h"

#include "deps\glm\glm.hpp"
#include "deps\glm\gtc\matrix_transform.hpp"

enum CameraMovement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		ROLL_LEFT,
		ROLL_RIGHT,
		SPRINT,
		WALK
};

const GLfloat SPEED = 7.0f;
const GLfloat SENSITIVITY = 0.25f;
const GLfloat ROLL_SPEED = 1.5f;
const GLfloat ZOOM = 45.0f;

class Camera {


public:

	Camera(glm::vec4 position = glm::vec4(0.0f, 15.0f, 0.0f, 0.0f), glm::vec4 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)) 
		: rotation(glm::mat4(1.0)), front(glm::vec4(0.0f, -1.0f, 0.0f, 0.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {

		this->position = position;
		this->worldUp = up;
		this->updateCameraVectors();
	}

	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ) 
		: rotation(glm::mat4(1.0)), front(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {

		this->position = glm::vec4(posX, posY, posZ, 1.0f);
		this->worldUp = glm::vec4(upX, upY, upZ, 0.0f);
		this->updateCameraVectors();
	}

	glm::mat4 getViewMatrix() {
		return glm::lookAt(glm::vec3(this->position), glm::vec3(this->position + this->front), glm::vec3(this->up));
	}

	void processKeyboard(CameraMovement command, GLfloat deltaTime) {
		GLfloat velocity = this->movementSpeed * deltaTime;

		if (FORWARD == command) {
			this->position += this->front * velocity;
		}

		if (BACKWARD == command) {
			this->position -= this->front * velocity;
		}

		if (LEFT == command) {
			this->position -= this->right * velocity;
		}

		if (RIGHT == command) {
			this->position += this->right * velocity;
		}

		if (UP == command) {
			this->position += this->up * velocity;
		}

		if (DOWN == command) {
			this->position -= this->up * velocity;
		}

		if (ROLL_LEFT == command) {
			this->worldUp = glm::rotate(glm::mat4(1), -glm::radians(ROLL_SPEED), glm::vec3(this->front)) * this->worldUp;
			updateCameraVectors();
		}

		if (ROLL_RIGHT == command) {
			this->worldUp = glm::rotate(glm::mat4(1), glm::radians(ROLL_SPEED), glm::vec3(this->front)) * this->worldUp;
			updateCameraVectors();
		}

		if (SPRINT == command) {
			this->movementSpeed = SPEED * 2.5;
		}

		if (WALK == command) {
			this->movementSpeed = SPEED;
		}

	}

	void processMouseMovement(GLfloat xOffset, GLfloat yOffset) {

		xOffset *= this->mouseSensitivity;
		this->rotation = glm::rotate(glm::mat4(1), -glm::radians(xOffset), glm::vec3(this->up)) * this->rotation;
		this->updateCameraVectors();

		yOffset *= this->mouseSensitivity;
		this->rotation = glm::rotate(glm::mat4(1), glm::radians(yOffset), glm::vec3(this->right)) * this->rotation;
		this->updateCameraVectors();
	}

	void processMouseScroll(GLfloat yOffset) {
		if (yOffset > 0) {
			smoothZoom += 3;
		}
		else if (yOffset < 0) {
			smoothZoom -= 3;
		}
	}

	GLfloat getSmoothedZoom() {
		if (smoothZoom > 0) {
			if (this->zoom >= 5.0f && this->zoom <= 45.0f) {
				this->zoom -= 1.2f;
			}
			if (this->zoom <= 5.0f) {
				this->zoom = 5.0f;
			}
			if (this->zoom >= 45.0f) {
				this->zoom = 45.0f;
			}
			smoothZoom--;
		}
		else if (smoothZoom < 0) {
			if (this->zoom >= 5.0f && this->zoom <= 45.0f) {
				this->zoom += 1.2f;
			}
			if (this->zoom <= 5.0f) {
				this->zoom = 5.0f;
			}
			if (this->zoom >= 45.0f) {
				this->zoom = 45.0f;
			}
			smoothZoom++;
		}

		this->mouseSensitivity = 0.25 * (this->zoom / 45);

		return this->zoom;
	}


private:

	glm::vec4 position;
	glm::vec4 front;
	glm::vec4 up;
	glm::vec4 right;
	glm::vec4 worldUp;
	glm::mat4 rotation;


	GLfloat movementSpeed;
	GLfloat mouseSensitivity;
	GLfloat zoom;
	GLint smoothZoom = 0;

	void updateCameraVectors() {
		
		std::cout << position.x << " " << position.y << " " << position.z << std::endl;
		std::cout << front.x << " " << front.y << " " << front.z << std::endl;
		
		this->front = rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
		this->right = glm::vec4(glm::normalize(glm::cross(glm::vec3(this->front), glm::vec3(this->worldUp))), 0.0f);
		this->up = glm::vec4(glm::normalize(glm::cross(glm::vec3(this->right), glm::vec3(this->front))), 0.0f);
	}

};