#define GLEW_STATIC

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "deps/glm/glm.hpp"
#include "deps/glm/gtc/matrix_transform.hpp"
#include "deps/glm/gtc/type_ptr.hpp"
#include "deps/glm/gtx/rotate_vector.hpp"

#include "Shader.h"

const int WIDTH = 800, HEIGHT = 800;
int SCREEN_WIDTH, SCREEN_HEIGHT;

float deltaTime, lastFrame;
double mX, mY;

GLuint VAO, VBO;

std::vector<glm::vec3> vertices;

void initializeBuffers(std::vector<glm::vec3> vertices) {

	// Buffer initialization
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(GLfloat), &(vertices.front()), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

}

void subdivideSpline(float u0, float u1, float minU, glm::mat4x3 f, std::vector<glm::vec3>* sV) {
	float uMid = (u0 + u1) / 2;
	glm::vec3 v0 = f * glm::vec4(glm::pow(u0, 3), glm::pow(u0, 2), u0, 1);
	glm::vec3 v1 = f * glm::vec4(glm::pow(u1, 3), glm::pow(u1, 2), u1, 1);
	sV->push_back(v0);
	sV->push_back(v1);
	if (glm::length(v1 - v0) > 0.1f) {
		subdivideSpline(u0, uMid, minU, f, sV);
		subdivideSpline(uMid, u1, minU, f, sV);
	}
}

std::vector<glm::vec3> reticulateSplines(std::vector<glm::vec3> vertices) {
	float s = 0.5;
	glm::mat4x3 cm;
	glm::mat4 basis(
		-s, (2-s), (s-2), s,
		(2*s), (s-3), (3-(2*s)), -s,
		-s, 0, s, 0,
		0, 1, 0, 0
	);
	std::vector<glm::vec3> splineVertices;
	if (vertices.size() > 3) {
		for (int i = 0, n = vertices.size(); i < (n - 3); i++) {
			cm = glm::mat4x3(
				vertices[i].x, vertices[i].y, vertices[i].z,
				vertices[i+1].x, vertices[i+1].y, vertices[i+1].z,
				vertices[i+2].x, vertices[i+2].y, vertices[i+2].z,
				vertices[i+3].x, vertices[i+3].y, vertices[i+3].z
			);
			
			subdivideSpline(0.0f, 1.0f, 0.1f, cm*basis, &splineVertices);
		}
	}
	return splineVertices;
}



void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		vertices = reticulateSplines(vertices);
		initializeBuffers(vertices);
	}
}
void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	mX = xPos;
	mY = yPos;
};
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		GLfloat nX = (2 * mX) / SCREEN_WIDTH - 1;
		GLfloat nY = -((2 * mY) / SCREEN_HEIGHT - 1);
		vertices.push_back(glm::vec3(nX, nY, 0.0f));
		initializeBuffers(vertices);
	}
};
void windowResizeCallback(GLFWwindow* window, int width, int height) {
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	glViewport(0, 0, width, height);
}

int main() {
	// GLFW settings
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment 1", nullptr, nullptr);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();

		return 1;
	}

	glfwMakeContextCurrent(window);

	// Set callback functions
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetWindowSizeCallback(window, windowResizeCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glPointSize(5);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;

		return 1;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glEnable(GL_DEPTH_TEST);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	Shader myShader("res/shaders/vertex.shader", "res/shaders/fragment.shader");

	
	if (vertices.size() > 0)
		initializeBuffers(vertices);

	// Color attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GL_FLOAT)));
	//glEnableVertexAttribArray(1);
	//glBindVertexArray(0);



	// Game loop
	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activated
		glfwPollEvents();


		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		myShader.use();

		/*
		glm::mat4 model;
		model = glm::mat4(1.0f);

		glm::mat4 view;
		view = glm::lookAt(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 projection;
		projection = glm::ortho(10.0f, 10.0f, 10.0f, 10.0f);

		//broadcast the uniform values to the shaders
		GLuint modelLoc = glGetUniformLocation(myShader.program, "model");
		GLuint viewLoc = glGetUniformLocation(myShader.program, "view");
		GLuint projectionLoc = glGetUniformLocation(myShader.program, "projection");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		*/


		if (vertices.size() > 0) {
			glBindVertexArray(VAO);

			glDrawArrays(GL_POINTS, 0, vertices.size());

			glBindVertexArray(0);
		}

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();

	return 0;

}