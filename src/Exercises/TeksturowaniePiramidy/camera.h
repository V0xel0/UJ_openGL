#pragma once
#include <cmath>

class Camera
{
public:

	void set_look_at(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up)
	{
		// axis_y is "up", axis_z is "depth"
		m_position = eye;
		m_target = center;
		m_axis_z = glm::normalize(m_position - m_target);
		m_axis_x = glm::normalize(glm::cross(up, m_axis_z));
		m_axis_y = glm::cross(m_axis_z, m_axis_x);
	}

	void set_perspective(float fov, float aspect, float near, float far)
	{
		m_fov = fov;
		m_aspect = aspect;
		m_near = near;
		m_far = far;
	}

	void set_aspect(float aspect)
	{
		m_aspect = aspect;
	}

	glm::mat4 create_view() const
	{
		glm::mat4 V(1.0f);
		for (int i = 0; i < 3; ++i)
		{
			V[i][0] = m_axis_x[i];
			V[i][1] = m_axis_y[i];
			V[i][2] = m_axis_z[i];
		}

		auto t = -glm::vec3{
		   glm::dot(m_axis_x, m_position),
		   glm::dot(m_axis_y, m_position),
		   glm::dot(m_axis_z, m_position),
		};
		V[3] = glm::vec4(t, 1.0f);

		return V;
	}

	glm::mat4 create_projection() const
	{
		return glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
	}

	void zoom(float y_offset)
	{
		// This is much faster ans simpler solution than proposed one
#if 1
		m_fov -= (float)y_offset*5.5f; // float literal is just for speed of zooming
		m_fov = glm::clamp(m_fov, 1.0f, 120.0f);
#endif
		// Expected solution
#if 0
		auto radFov = glm::radians(m_fov);
		auto x = radFov / glm::pi<float>();
		auto y = std::log( x / (1.0f - x) );
		y -= ( y_offset / 30);
		x = 1.0f / ( 1.0f + std::exp(-y) );
		radFov = x * glm::pi<float>();
		m_fov = glm::degrees(radFov);
#endif
	}

	glm::vec3 get_axis_x() const
	{
		return m_axis_x;
	}
	glm::vec3 get_axis_y() const
	{
		return m_axis_y;
	}
	glm::vec3 get_direction() const
	{
		return m_axis_z;
	}
	glm::vec3 get_position() const
	{
		return m_position;
	}
	glm::vec3 get_target() const
	{
		return m_target;
	}

	void rotate_around_target(float angle, const glm::vec3 &axis)
	{
		rotate_around(angle, axis, m_target);
	}

	void toggleRotation()
	{
		m_free_rotation = !m_free_rotation;
	}

	void move_camera_forward(float speed = 0.05f)
	{
		m_position -= speed * m_axis_z;
	}

	void move_camera_backward(float speed = 0.05f)
	{
		m_position += speed * m_axis_z;
	}

	void move_camera_right(float speed = 0.05f)
	{
		m_position -= glm::normalize(glm::cross(m_axis_z, { 0.0f, 1.0f, 0.0f })) * speed;
	}

	void move_camera_left(float speed = 0.05f)
	{
		m_position += glm::normalize(glm::cross(m_axis_z, { 0.0f, 1.0f, 0.0f })) * speed;
	}

	void move_camera_up(float speed = 0.05f)
	{
		m_position += glm::vec3(0.0f, 1.0f, 0.0f) * speed;
	}

	void move_camera_down(float speed = 0.05f)
	{
		m_position -= glm::vec3(0.0f, 1.0f, 0.0f) * speed;
	}

private:
	float m_fov;
	float m_aspect;
	float m_near;
	float m_far;
	
	glm::vec3 m_target;
	glm::vec3 m_position;
	glm::vec3 m_axis_x;
	glm::vec3 m_axis_y;
	glm::vec3 m_axis_z;

	bool m_free_rotation = false;

	glm::mat3 rotation(float angle, const glm::vec3 &axis)
	{
		auto u = glm::normalize(axis);
		auto s = std::sin(angle);
		auto c = std::cos(angle);

		return glm::mat3(
			c + u.x * u.x * (1.0f - c),
			u.y * u.x * (1.0f - c) + u.z * s,
			u.z * u.x * (1.0f - c) - u.y * s,

			u.x *u.y*(1.0f - c) - u.z *s,
			c + u.y*u.y *(1.0f - c),
			u.z*u.y*(1.0f - c) + u.x*s,

			u.x*u.z*(1.0f - c) + u.y*s,
			u.y*u.z*(1.0f - c) - u.x*s,
			c + u.z*u.z*(1.0f - c)
		);
	}

	void rotate_around(float angle, const glm::vec3 &axis, const glm::vec3 &c)
	{
		glm::mat3 R = rotation(angle, axis);
		m_axis_x = R * m_axis_x;
		m_axis_y = R * m_axis_y;
		m_axis_z = R * m_axis_z;

		if (!m_free_rotation)
		{
			glm::vec3 t = m_position - c;
			t = R * t;
			m_position = c + t;
		}
	}
};