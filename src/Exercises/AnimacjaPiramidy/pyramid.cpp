#include "pyramid.h"
#include <cassert>

Pyramid::Pyramid(std::vector<GLfloat> *verts, std::vector<GLuint> *ids) :
	vert_data(verts), indices(ids)
{
	//(note) Doing stuff that handles external resource in constructor is not a good practice due
	// to possible problems with RVO/NRVO - compiler can skip creating object if it is redundant
	// even if constructor and/or destructor have side effects! Also we could easily construct/destruct
	// two times for external resource - cause of creation of temp objects in some containers (like std::vector)

	assert(verts != nullptr && ids != nullptr && "Invalid data in constructor");

	GLuint vertexSize = 2 * sizeof(glm::vec3);
	hasBeenStolen = false;

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
	//(note) Removing in destructor is not ideal, see: https://www.khronos.org/opengl/wiki/Common_Mistakes - we need to be sure, that
	// openGL context is still valid/active and that it was properly created
	// Also it very very prone to pre-deletion bug if used in owning container (like std::vector) !

	// If we haven't given out control of external resource to other 
	if(!hasBeenStolen)
	{
		glDeleteBuffers(1, &buffers_handles[VBO]);
		glDeleteBuffers(1, &buffers_handles[EBO]);
		glDeleteVertexArrays(1, &vao_handle);
	}
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
