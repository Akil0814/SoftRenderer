#include "camera.h"

#include <cmath>

namespace
{
	constexpr float k_parallel_threshold = 1e-6f;
	constexpr float k_min_distance_to_target = 0.1f;
	const mai::vec3f k_world_up{ 0.0f, 1.0f, 0.0f };

	mai::vec3f transform_direction(const mai::mat4f& matrix, const mai::vec3f& direction)
	{
		const mai::vec4f result = matrix * mai::vec4f(direction.x, direction.y, direction.z, 0.0f);
		return mai::vec3f(result.x, result.y, result.z);
	}

	mai::vec3f normalized_or_default(const mai::vec3f& value, const mai::vec3f& fallback)
	{
		if (mai::length_squared(value) <= k_parallel_threshold)
			return fallback;
		return mai::normalize(value);
	}
}

Camera::Camera(float fovy, float aspect, float near_clip, float far_clip, const mai::vec3f& top)
{
	_up = normalized_or_default(top, k_world_up);
	_projection_matrix = mai::perspective(fovy, aspect, near_clip, far_clip);
	update_view_matrix();
}

void Camera::on_l_mouse_down(int x, int y)
{
	_left_mouse_down = true;
	_current_mouse_x = x;
	_current_mouse_y = y;
}

void Camera::on_l_mouse_up(int x, int y)
{
	(void)x;
	(void)y;
	_left_mouse_down = false;
}

void Camera::on_m_mouse_down(int x, int y)
{
	_middle_mouse_down = true;
	_current_mouse_x = x;
	_current_mouse_y = y;
}

void Camera::on_m_mouse_up(int x, int y)
{
	(void)x;
	(void)y;
	_middle_mouse_down = false;
}

void Camera::on_r_mouse_down(int x, int y)
{
	(void)x;
	(void)y;
}

void Camera::on_r_mouse_up(int x, int y)
{
	(void)x;
	(void)y;
}

void Camera::on_mouse_move(int x, int y)
{
	const int x_offset = x - _current_mouse_x;
	const int y_offset = y - _current_mouse_y;

	if (_left_mouse_down)
	{
		pitch(static_cast<float>(-y_offset) * _sensitivity);
		yaw(static_cast<float>(-x_offset) * _sensitivity);
	}
	else if (_middle_mouse_down)
	{
		const float delta_x = static_cast<float>(x_offset) * _move_speed;
		const float delta_y = static_cast<float>(y_offset) * _move_speed;
		const mai::vec3f movement = _up * delta_y - _right * delta_x;

		_position += movement;
		_target += movement;
		update_view_matrix();
	}

	_current_mouse_x = x;
	_current_mouse_y = y;
}

void Camera::on_mouse_wheel(float offset)
{
	update_basis();

	const float current_distance = mai::length(_target - _position);
	if (current_distance <= k_min_distance_to_target)
		return;

	float delta_distance = _scale_speed * offset;
	if (delta_distance > current_distance - k_min_distance_to_target)
		delta_distance = current_distance - k_min_distance_to_target;

	_position += _front * delta_distance;
	update_view_matrix();
}

void Camera::on_key_down(uint32_t key)
{
	(void)key;
}

void Camera::on_key_up(uint32_t key)
{
	(void)key;
}

void Camera::set_position(const mai::vec3f& position)
{
	_position = position;
	update_view_matrix();
}

void Camera::set_target(const mai::vec3f& target)
{
	_target = target;
	update_view_matrix();
}

void Camera::update_basis()
{
	_front = normalized_or_default(_target - _position, mai::vec3f{ 0.0f, 0.0f, -1.0f });

	mai::vec3f right = mai::cross(_front, _up);
	if (mai::length_squared(right) <= k_parallel_threshold)
		right = mai::cross(_front, std::fabs(_front.y) > 0.99f ? mai::vec3f{ 0.0f, 0.0f, 1.0f } : k_world_up);

	_right = normalized_or_default(right, mai::vec3f{ 1.0f, 0.0f, 0.0f });
	_up = mai::normalize(mai::cross(_right, _front));
}

void Camera::update_view_matrix()
{
	update_basis();

	_view_matrix = mai::mat4f(1.0f);
	_view_matrix.set(0, 0, _right.x);
	_view_matrix.set(0, 1, _right.y);
	_view_matrix.set(0, 2, _right.z);
	_view_matrix.set(0, 3, -mai::dot(_right, _position));

	_view_matrix.set(1, 0, _up.x);
	_view_matrix.set(1, 1, _up.y);
	_view_matrix.set(1, 2, _up.z);
	_view_matrix.set(1, 3, -mai::dot(_up, _position));

	_view_matrix.set(2, 0, -_front.x);
	_view_matrix.set(2, 1, -_front.y);
	_view_matrix.set(2, 2, -_front.z);
	_view_matrix.set(2, 3, mai::dot(_front, _position));
}

void Camera::pitch(float angle)
{
	const mai::mat4f rotation = mai::rotate(mai::mat4f(1.0f), static_cast<float>(MAI_DEG2RAD(angle)), _right);
	const mai::vec3f orbit = transform_direction(rotation, _position - _target);

	_position = _target + orbit;
	_up = normalized_or_default(transform_direction(rotation, _up), _up);
	update_view_matrix();
}

void Camera::yaw(float angle)
{
	const mai::mat4f rotation = mai::rotate(mai::mat4f(1.0f), static_cast<float>(MAI_DEG2RAD(angle)), k_world_up);
	const mai::vec3f orbit = transform_direction(rotation, _position - _target);

	_position = _target + orbit;
	_up = normalized_or_default(transform_direction(rotation, _up), _up);
	_right = normalized_or_default(transform_direction(rotation, _right), _right);
	update_view_matrix();
}

void Camera::update(float delta_time)
{
	(void)delta_time;
	update_view_matrix();
}
