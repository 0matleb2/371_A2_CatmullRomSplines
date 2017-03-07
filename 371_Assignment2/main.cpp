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

#include "Renderer.h"
#include "FileIO.h"

enum GameState {INPUT_PROFILE, INPUT_TRAJECTORY, VIEW_PROFILE, VIEW_TRAJECTORY, VIEW_MESH};
enum SweepType {TRANSLATIONAL, ROTATIONAL};

GameState currentState;
SweepType sweepType;

const int WIDTH = 800, HEIGHT = 800;
int SCREEN_WIDTH, SCREEN_HEIGHT;

float deltaTime, lastFrame;

double mX, mY;

GLuint VAO, VBO;

std::vector<glm::vec3> vertices, profile, trajectory;
std::vector<GLuint> indices;



glm::vec3 catmullRom(float u, glm::mat4x3 control, float tension = 0.5) {

	glm::mat4 basis = glm::mat4(
		-tension, (2 - tension), (tension - 2), tension,
		(2 * tension), (tension - 3), (3 - (2 * tension)), -tension,
		-tension, 0, tension, 0,
		0, 1, 0, 0
	);

	glm::vec4 parameter = glm::vec4(glm::pow(u, 3), glm::pow(u, 2), u, 1);

	return control * basis * parameter;
}



void subdivideSpline(float u0, float u1, float minU, glm::mat4x3 control, std::vector<glm::vec3>* sV) {

	float uMid = (u0 + u1) / 2;

	glm::vec3 v0 = catmullRom(u0, control);
	glm::vec3 v1 = catmullRom(u1, control);


	if (glm::length(v1 - v0) > 0.1f) {
		subdivideSpline(u0, uMid, minU, control, sV);
		subdivideSpline(uMid, u1, minU, control, sV);
	}
	else {
		sV->push_back(v1);
	}

}


std::vector<glm::vec3> generateSplines(std::vector<glm::vec3> inputVertices) {

	std::vector<glm::vec3> splineVertices;

	if (inputVertices.size() > 3) {

		// Push first vertex
		splineVertices.push_back(inputVertices[1]);

		// Generate a spline for each 4 control points
		for (int i = 0, n = vertices.size(); i < (n - 3); i++) {

			glm::mat4x3 control = glm::mat4x3(
				vertices[i].x,			vertices[i].y,			vertices[i].z,
				vertices[i + 1].x,		vertices[i + 1].y,		vertices[i + 1].z,
				vertices[i + 2].x,		vertices[i + 2].y,		vertices[i + 2].z,
				vertices[i + 3].x,		vertices[i + 3].y,		vertices[i + 3].z
			);


			
			// Pushes middle vertices
			subdivideSpline(0.0f, 1.0f, 0.1f, control, &splineVertices);


		}
	}
	return splineVertices;
}






void loadMeshData() {

	std::vector<std::string> input_file_lines = FileIO::readFile();


	std::vector<glm::vec3> surfaceVertices;
	std::vector<GLuint> surfaceIndices;
	int numPointsProfile, numPointsTrajectory, rotationalSpan;

	// Translational sweep
	if (input_file_lines[0] == "0") {
		std::cout << "Translational:" << std::endl;

		// Get profile curve

		std::cout << "Profile:" << std::endl;
		numPointsProfile = std::stoi(input_file_lines[1]);
		std::vector<glm::vec3> profileCurveVertices;

		for (int i = 0; i < numPointsProfile; i++) {

			std::stringstream lineStream(input_file_lines[i + 2]);

			float x;
			lineStream >> x;
			float y;
			lineStream >> y;
			float z;
			lineStream >> z;

			profileCurveVertices.push_back(glm::vec3(x, y, z));
			std::cout << x << ", " << y << ", " << z << std::endl;
		}

		// Get trajectory curve

		std::cout << "Trajectory:" << std::endl;
		numPointsTrajectory = std::stoi(input_file_lines[1 + numPointsProfile + 1]);
		std::vector<glm::vec3> trajectoryCurveVertices;

		for (int i = 0; i < numPointsTrajectory; i++) {

			std::stringstream lineStream(input_file_lines[2 + numPointsProfile + 1 + i]);

			float x;
			lineStream >> x;
			float y;
			lineStream >> y;
			float z;
			lineStream >> z;

			trajectoryCurveVertices.push_back(glm::vec3(x, y, z));
			std::cout << x << ", " << y << ", " << z << std::endl;
		}

		// Create vertice array

		for (int i = 0, n = profileCurveVertices.size(); i < n; ++i) { // Original profile
			surfaceVertices.push_back(profileCurveVertices[i]);
		}

		for (int i = 1; i < numPointsTrajectory; ++i) { // Translated profiles

			glm::vec3 translation = trajectoryCurveVertices[i] - trajectoryCurveVertices[0];

			for (int j = 0, n = profileCurveVertices.size(); j < n; ++j) {
				glm::vec3 translatedVertex = profileCurveVertices[j] + translation;
				surfaceVertices.push_back(translatedVertex);
			}
		}

		std::cout << "Vector surface vertices" << std::endl;
		for (int i = 0, n = surfaceVertices.size(); i < n; ++i) {
			std::cout << surfaceVertices[i].x << ", " << surfaceVertices[i].y << ", " << surfaceVertices[i].z << std::endl;
		}

		// Create indice array
		std::cout << "Surface indices:" << std::endl;
		for (int i = numPointsProfile, n = surfaceVertices.size() - 1; i < n; ++i) {
			if ((i + 1) % numPointsProfile != 0) {
				std::cout << i << ", " << i - numPointsProfile << ", " << i - numPointsProfile + 1 << std::endl;
				surfaceIndices.push_back(i);
				surfaceIndices.push_back(i - numPointsProfile);
				surfaceIndices.push_back(i - numPointsProfile + 1);

				std::cout << i << ", " << i - numPointsProfile + 1 << ", " << i + 1 << std::endl;
				surfaceIndices.push_back(i);
				surfaceIndices.push_back(i - numPointsProfile + 1);
				surfaceIndices.push_back(i + 1);
			}
		}

	}



	// Rotational sweep
	else {
		std::cout << "Rotational:" << std::endl;

		// Get profile curve
		std::cout << "Profile:" << std::endl;
		rotationalSpan = std::stoi(input_file_lines[1]);
		numPointsProfile = std::stoi(input_file_lines[2]);
		std::vector<glm::vec3> profileCurveVertices;

		for (int i = 0; i < numPointsProfile; i++) {

			std::stringstream lineStream(input_file_lines[i + 3]);

			float x;
			lineStream >> x;
			float y;
			lineStream >> y;
			float z;
			lineStream >> z;

			profileCurveVertices.push_back(glm::vec3(x, y, z));
			surfaceVertices.push_back(glm::vec3(x, y, z));
			std::cout << x << ", " << y << ", " << z << std::endl;
		}

		// Get rotated vertices
		for (int i = 0; i < rotationalSpan; ++i) {
			for (int j = 0, n = profileCurveVertices.size(); j < n; ++j) {
				glm::vec3 rotatedVertex = glm::rotateZ(profileCurveVertices[j], glm::radians((float(360) / rotationalSpan)*(i + 1)));
				surfaceVertices.push_back(rotatedVertex);
			}
		}

		std::cout << "Surface indices:" << std::endl;
		for (int i = numPointsProfile, n = surfaceVertices.size() - 1; i < n; ++i) {
			if ((i + 1) % numPointsProfile != 0) {
				//std::cout << i << ", " << i - numPointsProfile << ", " << i - numPointsProfile + 1 << std::endl;
				surfaceIndices.push_back(i);
				surfaceIndices.push_back(i - numPointsProfile);
				surfaceIndices.push_back(i - numPointsProfile + 1);

				//std::cout << i << ", " << i - numPointsProfile + 1 << ", " << i + 1 << std::endl;
				surfaceIndices.push_back(i);
				surfaceIndices.push_back(i - numPointsProfile + 1);
				surfaceIndices.push_back(i + 1);
			}
		}

	}


	vertices = surfaceVertices;
	indices = surfaceIndices;

}










void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);



	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		if (currentState == INPUT_PROFILE && sweepType == ROTATIONAL) {

			if (vertices.size() > 4) {
				vertices = generateSplines(vertices);
				Renderer::getInstance()->updateVertexBuffer(vertices);
			}

			FileIO::writeRotational(vertices);

			currentState = VIEW_PROFILE;

		}

		else if (currentState == INPUT_PROFILE && sweepType == TRANSLATIONAL) {

			if (vertices.size() > 4) {
				vertices = generateSplines(vertices);
				profile = vertices;

				Renderer::getInstance()->updateVertexBuffer(vertices);

				currentState = VIEW_PROFILE;
			}

		}

		else if (currentState == INPUT_TRAJECTORY && sweepType == TRANSLATIONAL) {

			if (vertices.size() > 4) {
				vertices = generateSplines(vertices);
				trajectory = vertices;

				Renderer::getInstance()->updateVertexBuffer(vertices);

				FileIO::writeTranslational(profile, trajectory);

				currentState = VIEW_TRAJECTORY;
			}

		}

		else if (currentState == VIEW_PROFILE && sweepType == ROTATIONAL) {

			vertices.clear();
			Renderer::getInstance()->updateVertexBuffer(vertices);

			loadMeshData();

			Renderer::getInstance()->updateVertexBuffer(vertices);
			Renderer::getInstance()->updateElementBuffer(indices);

			currentState = VIEW_MESH;
		}

		else if (currentState == VIEW_PROFILE && sweepType == TRANSLATIONAL) {

			vertices.clear();
			Renderer::getInstance()->updateVertexBuffer(vertices);

			currentState = INPUT_TRAJECTORY;

		}

		else if (currentState == VIEW_TRAJECTORY && sweepType == TRANSLATIONAL) {
			
			loadMeshData();

			Renderer::getInstance()->updateVertexBuffer(vertices);
			Renderer::getInstance()->updateElementBuffer(indices);
			
			currentState = VIEW_MESH;
		}
	}




	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		Renderer::getInstance()->setRenderMode(Renderer::LINES);
	}




	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		Renderer::getInstance()->setRenderMode(Renderer::POINTS);
	}
}




void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	mX = xPos;
	mY = yPos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		GLfloat nX = (2 * mX) / SCREEN_WIDTH - 1;
		GLfloat nY = -((2 * mY) / SCREEN_HEIGHT - 1);

		std::cout << "Mouse: (" << mX << ", " << mY << ")" << std::endl;
		std::cout << "Vertex: (" << nX << ", " << nY << ")" << std::endl;

		vertices.push_back(glm::vec3(nX, nY, 0.0f));
		Renderer::getInstance()->updateVertexBuffer(vertices);
	}
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	std::cout << SCREEN_HEIGHT << std::endl;
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	std::cout << SCREEN_HEIGHT << std::endl;
}



GLFWwindow* openWindow() {
	// Create GLFW window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "371_Assignment2", nullptr, nullptr);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();

		exit(1);
	}

	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);



	// Set callback functions
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetWindowSizeCallback(window, windowResizeCallback);


	// Initialize GLEW and OpenGL settings
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;

		exit(1);
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(5);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	return window;
}




int main() {


	char c;
	do {
		std::cout << "Translational (T) or rotational (R) sweep?" << std::endl;
		std::cin >> c;
	} while (c != 't' && c != 'T' && c != 'r' && c != 'R');

	if ( c == 't' || c == 'T')
		sweepType = TRANSLATIONAL;
	else if (c == 'r' || c == 'R')
		sweepType = ROTATIONAL;

	
	GLFWwindow* window = openWindow();


	// Game loop
	while (!glfwWindowShouldClose(window)) {

		std::cout << currentState << " " << sweepType << std::endl;

		/*
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		*/

		glfwPollEvents();

		if (currentState == VIEW_MESH)
			Renderer::getInstance()->renderElements();
		else
			Renderer::getInstance()->renderArrays();


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


		glfwSwapBuffers(window);
	}




	Renderer::getInstance()->terminate();
	glfwTerminate();

	return 0;

}