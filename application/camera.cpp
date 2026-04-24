#include "camera.h"

#include <cmath>

namespace
{
	constexpr float k_parallel_threshold = 1e-6f;
}

Camera::Camera(float fovy, float aspect, float near_clip, float far_clip, const mai::vec3f& top)
{
	if (mai::length_squared(top) > k_parallel_threshold)
		_top = mai::normalize(top);

	_projection_matrix = mai::perspective(fovy, aspect, near_clip, far_clip);
	update_front();
	update();
}

void Camera::on_r_mouse_down(int x, int y)
{
	_mouse_moving = true;
	_current_mouse_x = x;
	_current_mouse_y = y;
}

void Camera::on_r_mouse_up(int x, int y)
{
	(void)x;
	(void)y;
	_mouse_moving = false;
}

void Camera::on_mouse_move(int x, int y)
{
	if (_mouse_moving)
	{
		const int x_offset = x - _current_mouse_x;
		const int y_offset = y - _current_mouse_y;

		_current_mouse_x = x;
		_current_mouse_y = y;

		pitch(-y_offset);
		yaw(x_offset);
	}
}

void Camera::on_key_down(uint32_t key)
{
	switch (key)
	{
	case KEY_W:
		_move_state |= MOVE_FRONT;
		break;
	case KEY_A:
		_move_state |= MOVE_LEFT;
		break;
	case KEY_S:
		_move_state |= MOVE_BACK;
		break;
	case KEY_D:
		_move_state |= MOVE_RIGHT;
		break;
	default:
		break;
	}
}

void Camera::on_key_up(uint32_t key)
{
	switch (key)
	{
	case KEY_W:
		_move_state &= ~MOVE_FRONT;
		break;
	case KEY_A:
		_move_state &= ~MOVE_LEFT;
		break;
	case KEY_S:
		_move_state &= ~MOVE_BACK;
		break;
	case KEY_D:
		_move_state &= ~MOVE_RIGHT;
		break;
	default:
		break;
	}
}

void Camera::set_position(const mai::vec3f& position)
{
	_position = position;
	update_view_matrix();
}

void Camera::update_front()
{
	_front.y = static_cast<float>(std::sin(MAI_DEG2RAD(_pitch)));
	_front.x = static_cast<float>(std::cos(MAI_DEG2RAD(_yaw)) * std::cos(MAI_DEG2RAD(_pitch)));
	_front.z = static_cast<float>(std::sin(MAI_DEG2RAD(_yaw)) * std::cos(MAI_DEG2RAD(_pitch)));
	_front = mai::normalize(_front);
}

void Camera::update_view_matrix()
{
	const mai::vec3f forward = mai::normalize(_front);
	mai::vec3f right = mai::cross(forward, _top);

	if (mai::length_squared(right) <= k_parallel_threshold)
	{
		const mai::vec3f fallback_up = std::fabs(forward.y) > 0.99f
			? mai::vec3f{ 0.0f, 0.0f, 1.0f }
			: mai::vec3f{ 0.0f, 1.0f, 0.0f };
		right = mai::cross(forward, fallback_up);
	}

	right = mai::normalize(right);
	const mai::vec3f up = mai::normalize(mai::cross(right, forward));

	_view_matrix = mai::mat4f(1.0f);
	_view_matrix.set(0, 0, right.x);
	_view_matrix.set(0, 1, right.y);
	_view_matrix.set(0, 2, right.z);
	_view_matrix.set(0, 3, -mai::dot(right, _position));

	_view_matrix.set(1, 0, up.x);
	_view_matrix.set(1, 1, up.y);
	_view_matrix.set(1, 2, up.z);
	_view_matrix.set(1, 3, -mai::dot(up, _position));

	_view_matrix.set(2, 0, -forward.x);
	_view_matrix.set(2, 1, -forward.y);
	_view_matrix.set(2, 2, -forward.z);
	_view_matrix.set(2, 3, mai::dot(forward, _position));
}

void Camera::pitch(int yoffset)
{
	_pitch += yoffset * _sensitivity;

	if (_pitch >= 89.0f)
	{
		_pitch = 89.0f;
	}

	if (_pitch <= -89.0f)
	{
		_pitch = -89.0f;
	}

	update_front();
}

void Camera::yaw(int xoffset)
{
	_yaw += xoffset * _sensitivity;
	update_front();
}

void Camera::update(float delta_time)
{
	// Combine active input flags into a single movement direction.
	mai::vec3f move_direction = { 0.0f, 0.0f, 0.0f };

	const mai::vec3f front = _front;
	mai::vec3f right = mai::cross(_front, _top);
	if (mai::length_squared(right) > k_parallel_threshold)
		right = mai::normalize(right);
	else
		right = mai::vec3f{ 1.0f, 0.0f, 0.0f };

	if (_move_state & MOVE_FRONT)
	{
		move_direction += front;
	}

	if (_move_state & MOVE_BACK)
	{
		move_direction += -front;
	}

	if (_move_state & MOVE_LEFT)
	{
		move_direction += -right;
	}

	if (_move_state & MOVE_RIGHT)
	{
		move_direction += right;
	}

	if (mai::length_squared(move_direction) > 0.0f)
	{
		move_direction = mai::normalize(move_direction);
		_position += _speed * delta_time * move_direction;
	}

	update_view_matrix();
}
