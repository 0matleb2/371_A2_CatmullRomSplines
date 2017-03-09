#pragma once

#include <vector>
#include "deps\glm\gtc\matrix_transform.hpp"

#include "Shader.h"

class Renderer {


public:

	enum RenderMode { POINTS, LINES, FILL };

	static Renderer* getInstance();

	void renderArrays();

	void renderElements(glm::mat4 view, glm::mat4 projection);

	void updateVertexBuffer(std::vector<glm::vec3> vertices);

	void updateElementBuffer(std::vector<GLuint> indices);

	void setRenderMode(RenderMode mode);

	void terminate();

private:

	Renderer();

	static Renderer* instance;

	Shader* shader;

	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;

	GLuint VAO, VBO, EBO;

	RenderMode renderMode;


};