#pragma once

class CameraControler
{
public:
	CameraControler()
	{}
	CameraControler(Camera* camera) : m_camera(camera)
	{}
	void set_camera(Camera *camera)
	{
		m_camera = camera;
	}

	void rotate_camera(float dx, float dy)
	{
		m_camera->rotate_around_target(-m_scale * dy, m_camera->get_axis_x());
		m_camera->rotate_around_target(-m_scale * dx, glm::vec3{ 0.0f, 1.0f, 0.0f });
	}

	void mouse_moved(float x, float y)
	{
		if (m_LMB_pressed)
		{
			auto dx = x - m_prev_x;
			auto dy = y - m_prev_y;
			m_prev_x = x;
			m_prev_y = y;

			rotate_camera(dx, dy);
		}
	};

	void LMB_pressed(float x, float y)
	{
		m_LMB_pressed = true;
		m_prev_x = x;
		m_prev_y = y;
	};

	void LMB_released(float x, float y)
	{
		m_LMB_pressed = false;
		auto dx = x - m_prev_x;
		auto dy = y - m_prev_y;

		rotate_camera(dx, dy);
	};

private:
	Camera *m_camera = nullptr;
	bool m_LMB_pressed = false;

	float m_prev_x = 0.0f;
	float m_prev_y = 0.0f;

	float m_scale = 0.05f;
};