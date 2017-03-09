#include "Renderer.h"
#include "deps\glm\gtc\type_ptr.hpp"

Renderer *Renderer::instance = 0;


Renderer * Renderer::getInstance() {
	if (!instance) {
		instance = new Renderer();
	}
	return instance;
}

void Renderer::renderArrays() {

	glClearColor(0.01f, 0.01f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);

	if (renderMode == POINTS) {
		glDrawArrays(GL_POINTS, 0, vertices.size());
	}
	else if (renderMode == LINES) {
		glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
	}

	glBindVertexArray(0);

}

void Renderer::renderElements(glm::mat4 iView, glm::mat4 iProjection) {

	shader = new Shader("res/shaders/vertex2.shader", "res/shaders/fragment2.shader");
	shader->use();

	glClearColor(0.01f, 0.01f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 model = glm::scale(glm::mat4(1), glm::vec3(500, 500, 500));
	glm::mat4 view = iView;
	glm::mat4 projection = iProjection;

	//broadcast the uniform values to the shaders
	GLuint modelLoc = glGetUniformLocation(shader->program, "model");
	GLuint viewLoc = glGetUniformLocation(shader->program, "view");
	GLuint projectionLoc = glGetUniformLocation(shader->program, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(VAO);

	if (renderMode == POINTS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else if (renderMode == LINES) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else if (renderMode == FILL) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);

}

void Renderer::updateVertexBuffer(std::vector<glm::vec3> newVertices) {

	vertices = newVertices;

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Renderer::updateElementBuffer(std::vector<GLuint> newIndices) {
	indices = newIndices;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::setRenderMode(RenderMode mode) {
	renderMode = mode;
}

void Renderer::terminate() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	instance = 0;
}

Renderer::Renderer() {

	shader = new Shader("res/shaders/vertex.shader", "res/shaders/fragment.shader");
	shader->use();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	renderMode = POINTS;
}
