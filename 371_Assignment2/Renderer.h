#pragma once

#include <vector>
#include "deps\glm\gtc\matrix_transform.hpp"

#include "Shader.h"

class Renderer {


public:

	enum RenderMode { POINTS, LINES };

	static Renderer* getInstance();

	void render();

	void updateBuffer(std::vector<glm::vec3> vertices);

	void setRenderMode(RenderMode mode);

	void terminate();

private:

	Renderer();

	static Renderer* instance;

	Shader* shader;

	std::vector<glm::vec3> vertices;

	GLuint VAO;
	GLuint VBO;

	RenderMode renderMode;


};