#include "pyramid.h"
#include <cassert>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "3rdParty/src/stb/stb_image.h"

Pyramid::Pyramid()
{
	int width, height, n_channels;
	GLenum color_format = GL_RGB;
	const char *texture_filename = PROJECT_DIR "/Textures/multicolor.png";
	texture_data = stbi_load(texture_filename, &width, &height, &n_channels, 0);

	if (texture_data == nullptr || (width == 0 || height == 0 || n_channels == 0))
		std::cerr << "Failed to load texture or wrong texture format!\n";
	else
	{
		std::cout << "\nTexture loaded succesfully with:\n" << "h:" << height << "\nw:" <<
			width << "\nchannels:" << n_channels << "\n";
		if (n_channels > 3)
			color_format = GL_RGBA;
	}

	glGenTextures(1, &diff_texture_handle);
	glBindTexture(GL_TEXTURE_2D, diff_texture_handle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, color_format, GL_UNSIGNED_BYTE, texture_data);

	// Auto mip-map
	glGenerateMipmap(GL_TEXTURE_2D);

	//Wrapping settings:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//Filtering settings:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	vert_data = {
		// Pyramid face 1
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f,

		-0.5f, 0.0f, 0.5f,
		0.1910f, 0.5f,

		0.5f, 0.0f, 0.5f,
		0.5f, 0.1910f,

		0.5f, 0.0f, -0.5f,
		0.809f, 0.5f,

		-0.5f, 0.0f, -0.5f,
		0.5f, 0.809f,

		// rest of "upper"
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f,

		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f,
	};

	indices = {
		0, 1, 2,
		5, 2, 3,
		6, 3, 4,
		7, 4, 1,

		1, 4, 3,
		2, 1, 3
	};

	//(note) Doing stuff that handles external resource in constructor is not a good practice due
	// to possible problems with RVO/NRVO - compiler can skip creating object if it is redundant
	// even if constructor and/or destructor have side effects! Also we could easily construct/destruct
	// two times for external resource - cause of creation of temp objects in some containers (like std::vector)

	GLuint stride = sizeof(glm::vec3) + sizeof(glm::vec2);
	has_been_stolen = false;

	// Create VBO ID, activate, and allocate to GPU
	glGenBuffers(1, &buffers_handles[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers_handles[VBO]);
	glBufferData(GL_ARRAY_BUFFER, vert_data.size() * sizeof(GLfloat), vert_data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create EBO ID, activate, and allocate to GPU 
	glGenBuffers(1, &buffers_handles[EBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_handles[EBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	// Create VAO ID and activate first VAO
	glGenVertexArrays(1, &vao_handle);
	glBindVertexArray(vao_handle);

	// VBO activation, index and layout - vao gets handle to vbo with layout
	glBindBuffer(GL_ARRAY_BUFFER, buffers_handles[VBO]);
	// vert pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
	// vert uvs
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(sizeof(glm::vec3)));

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
	if(!has_been_stolen)
	{
		glDeleteBuffers(1, &buffers_handles[VBO]);
		glDeleteBuffers(1, &buffers_handles[EBO]);
		glDeleteVertexArrays(1, &vao_handle);
		glDeleteTextures(1, &diff_texture_handle);
		stbi_image_free(texture_data);
	}
}

void Pyramid::draw()
{
	if (!has_been_stolen)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diff_texture_handle);

		glBindVertexArray(vao_handle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_handles[EBO]);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}