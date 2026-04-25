#pragma once
#include "../MAI_SGL/base.h"
#include "../MAI_SGL/math/math.h"

class Camera {
public:
	Camera(float fovy, float aspect, float near_clip, float far_clip, const mai::vec3f& top);

	~Camera() = default;

public:
	void on_l_mouse_down(int x, int y);

	void on_l_mouse_up(int x, int y);

	void on_m_mouse_down(int x, int y);

	void on_m_mouse_up(int x, int y);

	void on_r_mouse_down(int x, int y);

	void on_r_mouse_up(int x, int y);

	void on_mouse_move(int x, int y);

	void on_mouse_wheel(float offset);

	void on_key_down(uint32_t key);

	void on_key_up(uint32_t key);

	void update(float delta_time = 1.0f / 60.0f);
	void set_position(const mai::vec3f& position);
	void set_target(const mai::vec3f& target);

	const mai::mat4f& get_view_matrix() const { return _view_matrix; }

	const mai::mat4f& get_projection_matrix() const { return _projection_matrix; }

private:
	void update_basis();
	void update_view_matrix();
	void pitch(float angle);
	void yaw(float angle);

private:
	mai::mat4f _view_matrix;
	mai::mat4f _projection_matrix;

	mai::vec3f _position{ 0.0f, 0.0f, 3.0f };
	mai::vec3f _target{ 0.0f, 0.0f, 0.0f };
	mai::vec3f _front{ 0.0f, 0.0f, -1.0f };
	mai::vec3f _up{ 0.0f, 1.0f, 0.0f };
	mai::vec3f _right{ 1.0f, 0.0f, 0.0f };

	bool _left_mouse_down{ false };
	bool _middle_mouse_down{ false };
	int _current_mouse_x{ 0 };
	int _current_mouse_y{ 0 };
	float _sensitivity{ 0.2f };
	float _move_speed{ 0.005f };
	float _scale_speed{ 0.2f };

};
