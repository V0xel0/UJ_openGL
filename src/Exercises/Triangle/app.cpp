//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>


#include "Application/utils.h"

void SimpleShapeApplication::init() {


	auto program = xe::create_program(std::string(PROJECT_DIR) + "/shaders/base_vs.glsl",
									  std::string(PROJECT_DIR) + "/shaders/base_fs.glsl");

	auto u_modifiers_index = glGetUniformBlockIndex(program, "Modifiers");

	if (u_modifiers_index == GL_INVALID_INDEX)
	{
		std::cout << "Cannot find Modifiers uniform block in program" << std::endl;
	}
	else
	{
		glUniformBlockBinding(program, u_modifiers_index, 0);
	}

	if (!program) {
		std::cerr << "Cannot create program from " << std::string(PROJECT_DIR) + "/shaders/base_vs.glsl" << " and ";
		std::cerr << std::string(PROJECT_DIR) + "/shaders/base_fs.glsl" << " shader files" << std::endl;
	}

	std::vector<GLfloat> vertices = {
		// Basic triangle
			-0.25f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,

			0.25f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,

			0.0f, 0.25f, 0.0f,
			1.0f, 0.0f, 0.0f,

		// house base
			-0.25f, -0.5f, 0.0f,
			0.0f, 1.0f, 0.0f,

			0.25f, -0.5f, 0.0f,
			0.0f, 1.0f, 0.0f,

		// fullscreen quad
#if 0
			-1.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f,

			1.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f,

			-1.0f, -1.0f, 0.0f,
			0.0f, 0.0f, 0.0f,

			1.0f, -1.0f, 0.0f,
			0.0f, 0.0f, 0.0f
#endif
	};

	std::vector<GLuint> indices = {
		// house
		0, 1, 2,
		0, 1, 4,
		0, 4, 3,

		// quad
		5, 6, 8,
		5, 8, 7
	};

	GLuint ubo_handle{};
	GLuint v_buffer_handle{};
	GLuint indices_handle{};
	GLuint vec3Size = 3 * sizeof(GLfloat);
	GLuint vertexSize = 2 * vec3Size;

	float uboData[] = { 1.7f, 1.0f, 0.95f, 1.0f };

	// UBO creation, copying data, linking
	glGenBuffers(1, &ubo_handle);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_handle);
	glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &uboData);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 12, &uboData[1]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_handle); // linking to binding point


	// Create VBO ID, activate, and allocate to GPU
	glGenBuffers(1, &v_buffer_handle);
	glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create EBO ID, activate, and allocate to GPU 
	glGenBuffers(1, &indices_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	// Create VAO ID and activate first VAO
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);
	
	// VBO activation, index and layout - vao gets handle to vbo with layout
	glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
	// vert pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(0));
	// vert colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(vec3Size) );

	// First EBO activation - vao gets handle to ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_handle);

	// Unbiding buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
	int w, h;
	std::tie(w, h) = frame_buffer_size();
	glViewport(0, 0, w, h);

	glEnable(GL_DEPTH_TEST);
	glUseProgram(program);
}

void SimpleShapeApplication::frame() {
	glBindVertexArray(vao_);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

	glBindVertexArray(0);
}
