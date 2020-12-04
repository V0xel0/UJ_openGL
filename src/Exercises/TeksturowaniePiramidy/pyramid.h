#pragma once

#include <vector>
#include "glad/glad.h"
#include "glm/glm.hpp"

// This class is basically a Mesh wrapper of openGL VAO, VBO and EBO handles
class Pyramid
{
	enum GL_Buffers
	{
		VBO,
		EBO,
		TypeCount
	};



public:
	Pyramid();
	~Pyramid();

	void draw();

private:
	GLuint vao_handle;
	GLuint buffers_handles[TypeCount];
	GLuint diff_texture_handle;

	std::vector<GLfloat> vert_data; 
	std::vector<GLuint> indices;
	uint8_t  *texture_data;

	bool has_been_stolen;

	void change_gl_handles_ownership(Pyramid& a, Pyramid& other)
	{
		a.vao_handle = other.vao_handle;
		a.buffers_handles[VBO] = other.buffers_handles[VBO];
		a.buffers_handles[EBO] = other.buffers_handles[EBO];
		a.diff_texture_handle = other.diff_texture_handle;
		a.vert_data.swap(other.vert_data);
		a.indices.swap(other.indices);
		a.texture_data = other.texture_data;
		other.has_been_stolen = true;
	}

public:
	// Rule of 5 - prevent copy-delete bugs and double contruction
	Pyramid(Pyramid&& other)
	{
		change_gl_handles_ownership(*this, other);
	}
	Pyramid& operator=(Pyramid&& other)
	{
		change_gl_handles_ownership(*this, other);
		return *this;
	}

	Pyramid(const Pyramid&) = delete; // prevent copy constructor to be used
	Pyramid& operator=(const Pyramid&) = delete; // prevent copy assignment to be used
};