//
// Created by pbialas on 05.08.2020.
//

#pragma once

#include <vector>

#include "Application/application.h"
#include "Application/utils.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "camera.h"
#include "cameraControler.h"

class SimpleShapeApplication : public xe::Application {
public:
	SimpleShapeApplication(int width, int height, std::string title, int major = 4, int minor = 1) :
			Application(width, height, title, major, minor) {}

	void init() override;
	void frame() override;
	void framebuffer_resize_callback(int w, int h) override;
	void scroll_callback(double xoffset, double yoffset) override;
	void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) override;
	void SimpleShapeApplication::cursor_position_callback(double x, double y) override;

private:
	GLuint vao_;
	GLuint ubo_handle_matrices_{};

	glm::mat4 mat_M_;

	Camera m_camera;
	CameraControler m_cam_control;
};