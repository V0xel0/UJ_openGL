#pragma once
#include <cmath>

class Camera
{
public:

	void set_look_at(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up)
	{
		V_ = glm::lookAt(eye, center, up);
	}

	void set_perspective(float fov, float aspect, float near, float far)
	{
		fov_ = fov;
		aspect_ = aspect;
		near_ = near;
		far_ = far;
	}

	void set_aspect(float aspect)
	{
		aspect_ = aspect;
	}

	glm::mat4 get_view() const
	{
		return V_;
	}

	glm::mat4 create_projection() const
	{
		return glm::perspective(glm::radians(fov_), aspect_, near_, far_);
	}

	void zoom(float y_offset)
	{
		// This is much faster ans simpler solution than proposed one
#if 1
		fov_ -= (float)y_offset*5.5f; // float literal is just for speed of zooming
		fov_ = glm::clamp(fov_, 1.0f, 180.0f);
#endif
		// Expected solution
#if 0
		auto radFov = glm::radians(fov_);
		auto x = radFov / glm::pi<float>();
		auto y = std::log( x / (1.0f - x) );
		y -= ( y_offset / 30);
		x = 1.0f / ( 1.0f + std::exp(-y) );
		radFov = x * glm::pi<float>();
		fov_ = glm::degrees(radFov);
#endif
	}

private:
	float fov_;
	float aspect_;
	float near_;
	float far_;

	glm::mat4 V_;
};