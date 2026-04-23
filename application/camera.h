#pragma once
#include "../MAI_SGL/base.h"
#include "../MAI_SGL/math/math.h"

#define NO_MOVE 0
#define MOVE_LEFT 0x001
#define MOVE_RIGHT 0x002
#define MOVE_FRONT 0x004
#define MOVE_BACK 0x008

#define KEY_W 0x57
#define KEY_A 0x41
#define KEY_S 0x53
#define KEY_D 0x44

class Camera {
public:
	Camera(float fovy, float aspect, float near_clip, float far_clip, const mai::vec3f& top);

	~Camera() = default;

public:
	void on_r_mouse_down(int x, int y);

	void on_r_mouse_up(int x, int y);

	void on_mouse_move(int x, int y);

	void on_key_down(uint32_t key);

	void on_key_up(uint32_t key);

	void update();

	const mai::mat4f& get_view_matrix() const { return _view_matrix; }

	const mai::mat4f& get_projection_matrix() const { return _projection_matrix; }

private:
	void update_front();
	void update_view_matrix();
	void pitch(int yoffset);
	void yaw(int xoffset);

private:
	mai::mat4f _view_matrix;
	mai::mat4f _projection_matrix;

	mai::vec3f _position{ 0.0f, 0.0f, 0.0f };
	mai::vec3f _front{ 0.0f, 0.0f, -1.0f };
	mai::vec3f _top{ 0.0f, 1.0f, 0.0f };
	float _speed = 0.01f;

	float _pitch{ 0.0f };
	float _yaw{ -90.0f };
	float _sensitivity{ 0.1f };

	uint32_t _move_state{ NO_MOVE };
	bool _mouse_moving{ false };
	int _current_mouse_x{ 0 };
	int _current_mouse_y{ 0 };

};
