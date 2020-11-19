#include "pyramid.h"
#include <cassert>

Pyramid::Pyramid(std::vector<GLfloat> *verts, std::vector<GLuint> *ids) :
	vert_data(verts), indices(ids)
{
	assert(verts != nullptr && ids != nullptr && "Invalid data in constructor");

	GLuint vertexSize = 2 * sizeof(glm::vec3);
	mat_M = glm::mat4(1.0f);

	// Create VBO ID, activate, and allocate to GPU
	glGenBuffers(1, &buffers_handles[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers_handles[VBO]);
	glBufferData(GL_ARRAY_BUFFER, vert_data->size() * sizeof(GLfloat), vert_data->data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create EBO ID, activate, and allocate to GPU 
	glGenBuffers(1, &buffers_handles[EBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_handles[EBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(GLuint), indices->data(), GL_STATIC_DRAW);

	// Create VAO ID and activate first VAO
	glGenVertexArrays(1, &vao_handle);
	glBindVertexArray(vao_handle);

	// VBO activation, index and layout - vao gets handle to vbo with layout
	glBindBuffer(GL_ARRAY_BUFFER, buffers_handles[VBO]);
	// vert pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(0));
	// vert colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(sizeof(glm::vec3)));

	// First EBO activation - vao gets handle to ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_handles[EBO]);

	// Unbiding buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Pyramid::~Pyramid()
{
	//Tu usuwamy VAO i bufory
	//(note) Removing in destructor is not ideal, see: https://www.khronos.org/opengl/wiki/Common_Mistakes - we need to be sure, that
	// openGL context is still valid/active and that it was properly created
}

void Pyramid::draw()
{
	glBindVertexArray(vao_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_handles[EBO]);

	glDrawArrays(GL_TRIANGLES, 0, 12);
	// Base of pyramid is being drawn with indices
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
