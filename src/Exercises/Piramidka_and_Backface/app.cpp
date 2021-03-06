//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>


#include "Application/utils.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

void SimpleShapeApplication::init()
{



	auto program = xe::create_program(std::string(PROJECT_DIR) + "/shaders/base_vs.glsl",
		std::string(PROJECT_DIR) + "/shaders/base_fs.glsl");

	auto u_modifiers_index = glGetUniformBlockIndex(program, "Modifiers");
	auto u_matrices_index = glGetUniformBlockIndex(program, "Matrices");

	if (u_modifiers_index == GL_INVALID_INDEX)
	{
		std::cout << "Cannot find Modifiers uniform block in program" << std::endl;
	}
	else
	{
		glUniformBlockBinding(program, u_modifiers_index, 0);
	}
	if (u_matrices_index == GL_INVALID_INDEX)
	{
		std::cout << "Cannot find Matrices uniform block in program" << std::endl;
	}
	else
	{
		glUniformBlockBinding(program, u_matrices_index, 1);
	}

	if (!program)
	{
		std::cerr << "Cannot create program from " << std::string(PROJECT_DIR) + "/shaders/base_vs.glsl" << " and ";
		std::cerr << std::string(PROJECT_DIR) + "/shaders/base_fs.glsl" << " shader files" << std::endl;
	}

	std::vector<GLfloat> vertices = {
			//// Pyramid face 1
			0.0f, 0.5f, 0.0f,
			1.0f, 0.0f, 0.0f,

			-0.5f, 0.0f, 0.5f,
			1.0f, 0.0f, 0.0f,

			0.5f, 0.0f, 0.5f,
			1.0f, 0.0f, 0.0f,

			// Pyramid face 2
			0.0f, 0.5f, 0.0f,
			0.0f, 1.0f, 0.0f,

			0.5f, 0.0f, 0.5f,
			0.0f, 1.0f, 0.0f,

			0.5f, 0.0f, -0.5f,
			0.0f, 1.0f, 0.0f,

			// Pyramid face 3
			0.0f, 0.5f, 0.0f,
			0.0f, 0.0f, 1.0f,

			0.5f, 0.0f, -0.5f,
			0.0f, 0.0f, 1.0f,

			-0.5f, 0.0f, -0.5f,
			0.0f, 0.0f, 1.0f,

			// Pyramid face 4
			0.0f, 0.5f, 0.0f,
			1.0f, 0.0f, 1.0f,

			-0.5f, 0.0f, -0.5f,
			1.0f, 0.0f, 1.0f,

			-0.5f, 0.0f, 0.5f,
			1.0f, 0.0f, 1.0f,

			// Pyramid base
			-0.5f, 0.0f, -0.5f,
			1.0f, 1.0f, 0.0f,

			-0.5f, 0.0f, 0.5f,
			1.0f, 1.0f, 0.0f,

			0.5f, 0.0f, 0.5f,
			1.0f, 1.0f, 0.0f,

			0.5f, 0.0f, -0.5f,
			1.0f, 1.0f, 0.0f,
	};

	std::vector<GLuint> indices = {
		12, 14, 13, 
		15, 14, 12
	};

	GLuint ubo_handle_color{};
	GLuint ubo_handle_matrices{};

	GLuint v_buffer_handle{};
	GLuint indices_handle{};
	GLuint vertexSize = 2 * sizeof(glm::vec3);

	float ubo_data_color[] = { 1.7f, 1.0f, 0.95f, 1.0f };

	// Matrices definition
	glm::mat4 mat_Model = glm::mat4(1.0f);
	glm::mat4 mat_View = glm::lookAt(
		glm::vec3(0.5f, 2.0f, 1.0f),  // cam position
		glm::vec3(0.0f, 0.0f, 0.0f),  // target position
		glm::vec3(0.0f, 1.0f, 0.0f)); // up vector
	glm::mat4 mat_Projection = glm::perspective(glm::radians(60.0f), 650.0f / 480.0f, 0.1f, 100.0f);
	glm::mat4 PVM = mat_Projection * mat_View * mat_Model;

	// 1. UBO creation, copying data, linking to binding point
	glGenBuffers(1, &ubo_handle_color);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_handle_color);
	glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &ubo_data_color);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 12, &ubo_data_color[1]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_handle_color);

	// 2. UBO creation, copying matrices, linking
	glGenBuffers(1, &ubo_handle_matrices);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_handle_matrices);
	glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &mat_Model[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), sizeof(glm::mat4), &mat_View[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), &mat_Projection[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), sizeof(glm::mat4), &PVM[0]);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_handle_matrices);

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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(sizeof(glm::vec3)));

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
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glUseProgram(program);
}

void SimpleShapeApplication::frame()
{
	glBindVertexArray(vao_);
	glDrawArrays(GL_TRIANGLES, 0, 12);
	// Base of pyramid is being drawn with indices
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

	glBindVertexArray(0);
}