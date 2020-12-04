//
// Created by pbialas on 25.09.2020.
//

#include "app.h"
#include <tuple>
#include "Application/utils.h"
#include <utility>

void SimpleShapeApplication::init()
{
	int w, h;
	std::tie(w, h) = frame_buffer_size();
	m_camera.set_perspective(60.0f, (float)w / h, 0.1f, 100.0f);
	m_cam_control.set_camera(&m_camera);
	m_camera.set_look_at(
		glm::vec3(0.0f, 3.0f, 1.0f),  // cam position
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

	GLuint ubo_handle_color{};

	pyramid = std::make_unique<Pyramid>();
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
}

void SimpleShapeApplication::frame()
{
	auto mat_P = m_camera.create_projection();
	auto mat_V = m_camera.create_view();
	auto mat_PV = mat_P * mat_V;

	auto mat_M = glm::mat4(1.0f);
	auto mat_PVM = mat_PV * mat_M;

	// Updating PVM UBO, Start of "UBO scope"
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_handle_matrices_);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &mat_M[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), sizeof(glm::mat4), &mat_V[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), &mat_P[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), sizeof(glm::mat4), &mat_PVM[0]);

	// Draw
	pyramid->draw();

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