#pragma once

#include <vector>
//#include <iostream>

#define GLEW_STATIC
#include <GL\glew.h>

#include <GLM\glm.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtc\quaternion.hpp>

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
		WALK,
		CRAWL
};

const GLfloat SPEED = 140.0f;
const GLfloat SENSITIVITY = 0.0025f;
const GLfloat ROLL_SPEED = 0.025f;
const GLfloat ZOOM = 45.0f;

class Camera {


public:

	Camera(glm::vec4 position = glm::vec4(0.0f, 0.0f, 400.0f, 0.0f), glm::quat cameraQuat = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)))
		: front(glm::vec3(0.0f, -1.0f, 0.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {

		this->position = position;
		this->cameraQuat = cameraQuat;
		this->updateView();
	}

	glm::mat4 getViewMatrix() {

		return viewMatrix;

		//return glm::lookAt(glm::vec3(this->position), glm::vec3(this->position + this->front), glm::vec3(this->up));
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
			this->position += this->right * velocity;
		}

		if (RIGHT == command) {
			this->position -= this->right * velocity;
		}

		if (UP == command) {
			this->position -= this->up * velocity;
		}

		if (DOWN == command) {
			this->position += this->up * velocity;
		}

		if (ROLL_LEFT == command) {
			keyRoll = -ROLL_SPEED;
			this->updateView();
		}

		if (ROLL_RIGHT == command) {
			keyRoll = +ROLL_SPEED;
			this->updateView();
		}

		if (SPRINT == command) {
			this->movementSpeed = SPEED * 2.5;
		}

		if (WALK == command) {
			this->movementSpeed = SPEED;
		}

		if (CRAWL == command) {
			this->movementSpeed = SPEED / 2.5;
		}

		updateView();
	}

	void processMouseMovement(GLfloat xOffset, GLfloat yOffset) {

		keyYaw = xOffset * this->mouseSensitivity;
		keyPitch = -yOffset * this->mouseSensitivity;

		this->updateView();

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

		this->mouseSensitivity = SENSITIVITY * (this->zoom / 45);

		return this->zoom;
	}


private:

	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;

	glm::quat cameraQuat;
	glm::vec3 eyeVector;
	GLfloat keyPitch, keyYaw, keyRoll;

	glm::mat4 viewMatrix;

	GLfloat movementSpeed;
	GLfloat mouseSensitivity;
	GLfloat zoom;
	GLint smoothZoom = 0;

	void updateView() {


		// Temporary frame quaternion from pitch, yaw, roll
		glm::quat key_quat = glm::quat(glm::vec3(keyPitch, keyYaw, keyRoll));
		// Reset frame keys;
		keyPitch = 0;
		keyYaw = 0;
		keyRoll = 0;

		cameraQuat = key_quat * cameraQuat;
		cameraQuat = glm::normalize(cameraQuat);

		glm::mat4 rotation = glm::mat4_cast(cameraQuat);

		glm::mat4 translation = glm::mat4(1.0f);
		translation = glm::translate(translation, position);

		viewMatrix = rotation * translation;

		glm::mat4 mat = viewMatrix;
		front = glm::vec3(mat[0][2], mat[1][2], mat[2][2]);
		right = glm::vec3(mat[0][0], mat[1][0], mat[2][0]);
		up = glm::normalize(glm::cross(front, right));
		
	}
};