//
// Created by pbialas on 25.09.2020.
//

#include "app.h"
#include <tuple>
#include "Application/utils.h"

void SimpleShapeApplication::init()
{
	int w, h;
	std::tie(w, h) = frame_buffer_size();
	m_camera.set_perspective(60.0f, (float)w / h, 0.1f, 100.0f);
	m_cam_control.set_camera(&m_camera);
	m_camera.set_look_at(
		glm::vec3(0.0f, 21.0f, 5.0f),  // cam position
		glm::vec3(0.0f, 0.0f, 0.0f),   // target position
		glm::vec3(0.0f, 1.0f, 0.0f));  // up vector)

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

	m_vertices = {
		// Pyramid face 1
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

	m_indices = {
		12, 14, 13, 
		15, 14, 12
	};

	GLuint ubo_handle_color{};

	// Pyramids(Mesh) creation
	pyramids.reserve(10);
	pyramids.push_back({ &m_vertices, &m_indices });

	float ubo_data_color[] = { 1.7f, 1.0f, 0.95f, 1.0f };

	// 1. UBO creation, copying data, linking to binding point
	glGenBuffers(1, &ubo_handle_color);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_handle_color);
	glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &ubo_data_color);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 12, &ubo_data_color[1]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_handle_color);

	// 2. UBO creation, 
	glGenBuffers(1, &ubo_handle_matrices_);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_handle_matrices_);
	glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_handle_matrices_);

	glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
	glViewport(0, 0, w, h);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glUseProgram(program);
	
	m_start = high_resolution_clock::now();
}

void SimpleShapeApplication::frame()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto elapsed_seconds = duration_cast<std::chrono::duration<float>>(now - m_start).count();

	auto mat_P = m_camera.create_projection();
	auto mat_V = m_camera.create_view();

	// Earth Orbit
	float earth_rotation_period = 4.0f;
	auto rotation_angle = 2.0f*glm::pi<float>()*elapsed_seconds / earth_rotation_period;
	float orbital_rotation_period = 20.0f;
	float a = 10.0f, b = 8.0f;
	float orbital_rotation_angle = 2.0f*glm::pi<float>()*elapsed_seconds / orbital_rotation_period;
	float x = a * cos(orbital_rotation_angle);
	float z = b * sin(orbital_rotation_angle);

	// Apply world (orbital) transformation to Earth
	glm::vec3 world_pos = { x, 0.0f, z };
	auto earth_orbit = glm::translate(glm::mat4(1.0f), world_pos);
	auto earth_rotate = glm::rotate(glm::mat4(1.0f), rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));

	auto mat_PV = mat_P * mat_V;
	auto mat_M = earth_orbit * earth_rotate;
	auto mat_PVM = mat_PV * mat_M;

	// Updating PVM UBO, Start of "UBO scope"
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_handle_matrices_);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &mat_M[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), sizeof(glm::mat4), &mat_V[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), &mat_P[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), sizeof(glm::mat4), &mat_PVM[0]);

	// Draw earth
	pyramids[0].draw();

	// Moon orbit
	float moon_r = 3.0f;
	float moon_rotation_period = 10.0;
	rotation_angle = 2.0f*glm::pi<float>()*elapsed_seconds / moon_rotation_period;
	orbital_rotation_angle = 2.0f*glm::pi<float>()*elapsed_seconds / moon_rotation_period;
	x = moon_r * cos(orbital_rotation_angle);
	z = moon_r * sin(orbital_rotation_angle);

	// Apply world (orbital) transformation to Moon
	world_pos = { x, 0.0f, z };
	auto moon_orbit = glm::translate(glm::mat4(1.0f), world_pos);
	auto moon_rotate = glm::rotate(glm::mat4(1.0f), rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	auto moon_scale = glm::scale(glm::mat4(1.0f), { 0.5f, 0.5f, 0.5f });

	mat_M = earth_orbit * moon_orbit * moon_rotate * moon_scale;
	mat_PVM = mat_PV * mat_M;

	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &mat_M[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), sizeof(glm::mat4), &mat_PVM[0]);

	// Draw moon
	pyramids[0].draw();

	// End of "UBO scope"
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h)
{
	Application::framebuffer_resize_callback(w, h);
	glViewport(0, 0, w, h);
	m_camera.set_aspect((float)w / h);
}

void SimpleShapeApplication::scroll_callback(double xoffset, double yoffset)
{
	Application::scroll_callback(xoffset, yoffset);
	m_camera.zoom(yoffset);
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods)
{
	double x, y;
	glfwGetCursorPos(window_, &x, &y);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		m_cam_control.LMB_pressed(x, y);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		m_cam_control.LMB_released(x, y);
}

void SimpleShapeApplication::cursor_position_callback(double x, double y)
{
	Application::cursor_position_callback(x, y);
		m_cam_control.mouse_moved(x, y);
}

void SimpleShapeApplication::key_callback(int key, int scancode, int action, int mods)
{
	Application::key_callback(key, scancode, action, mods);

	float camSpeed = 0.05f;

	if (key == GLFW_KEY_UP )
	{
		m_camera.move_camera_forward();
	}
	if (key == GLFW_KEY_DOWN )
	{
		m_camera.move_camera_backward();
	}	
	if (key == GLFW_KEY_LEFT )
	{
		m_camera.move_camera_left();
	}	
	if (key == GLFW_KEY_RIGHT)
	{
		m_camera.move_camera_right();
	}
	if (key == GLFW_KEY_PAGE_UP)
	{
		m_camera.move_camera_up();
	}
	if (key == GLFW_KEY_PAGE_DOWN)
	{
		m_camera.move_camera_down();
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		m_camera.toggleRotation();
	}
}