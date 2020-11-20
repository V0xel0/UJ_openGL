#pragma once

#include <vector>
#include "glad/glad.h"
#include "glm/glm.hpp"

class Pyramid
{
	enum GL_Buffers
	{
		VBO,
		EBO,
		TypeCount
	};

public:
	Pyramid(std::vector<GLfloat> *verts, std::vector<GLuint> *ids);
	~Pyramid();
	void draw();

private:
	GLuint vao_handle;
	GLuint buffers_handles[TypeCount];
	// not owning ptrs to raw data
	std::vector<GLfloat> *vert_data; 
	std::vector<GLuint> *indices;
};