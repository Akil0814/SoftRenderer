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
	Camera(float fovy, float aspect, float n, float f, const mai::vec3f& top);

	~Camera();

public:
	void onRMouseDown(const int& x, const int& y);

	void onRMouseUp(const int& x, const int& y);

	void onMouseMove(const int& x, const int& y);

	void onKeyDown(const uint32_t& key);

	void onKeyUp(const uint32_t& key);

	void update();

	mai::mat4f getViewMatrix()const { return mViewMatrix; }

	mai::mat4f getProjectionMatrix()const { return mProjectionMatrix; }

private:
	void pitch(int yoffset);

	void yaw(int xoffset);

private:
	mai::mat4f mViewMatrix;
	mai::mat4f mProjectionMatrix;

	mai::vec3f	mPosition{ 0.0f,0.0f,0.0f };
	mai::vec3f	mFront{ 0.0f,0.0f,-1.0f };
	mai::vec3f	mTop{ 0.0f,1.0f,0.0f };
	float		mSpeed = { 0.01f };

	float		mPitch{ 0.0f };
	float		mYaw{ -90.0f };
	float		mSensitivity{ 0.1f };

	uint32_t	mMoveState{ NO_MOVE };
	bool		mMouseMoving{ false };
	int			mCurrentMouseX{ 0 };
	int			mCurrentMouseY{ 0 };

};