#include "camera.h"

Camera::Camera(float fovy, float aspect, float n, float f, const mai::vec3f& top) {
	mTop = top;
	mProjectionMatrix = mai::perspective(fovy, aspect, n, f);
	update();
}

Camera::~Camera() {}

void Camera::onRMouseDown(const int& x, const int& y) {
	mMouseMoving = true;
	mCurrentMouseX = x;
	mCurrentMouseY = y;
}

void Camera::onRMouseUp(const int& x, const int& y) {
	mMouseMoving = false;
}

void Camera::onMouseMove(const int& x, const int& y) {
	if (mMouseMoving) {
		int xOffset = x - mCurrentMouseX;
		int yOffset = y - mCurrentMouseY;

		mCurrentMouseX = x;
		mCurrentMouseY = y;

		pitch(-yOffset);
		yaw(xOffset);
	}
}

void Camera::onKeyDown(const uint32_t& key) {
	switch (key) {
	case KEY_W:
		mMoveState |= MOVE_FRONT;
		break;
	case KEY_A:
		mMoveState |= MOVE_LEFT;
		break;
	case KEY_S:
		mMoveState |= MOVE_BACK;
		break;
	case KEY_D:
		mMoveState |= MOVE_RIGHT;
		break;
	default:
		break;
	}
}

void Camera::onKeyUp(const uint32_t& key) {
	switch (key) {
	case KEY_W:
		mMoveState &= ~MOVE_FRONT;
		break;
	case KEY_A:
		mMoveState &= ~MOVE_LEFT;
		break;
	case KEY_S:
		mMoveState &= ~MOVE_BACK;
		break;
	case KEY_D:
		mMoveState &= ~MOVE_RIGHT;
		break;
	default:
		break;
	}
}

void Camera::pitch(int yoffset) {
	mPitch += yoffset * mSensitivity;

	if (mPitch >= 89.0f)
	{
		mPitch = 89.0f;
	}

	if (mPitch <= -89.0f)
	{
		mPitch = -89.0f;
	}

	mFront.y = sin(MAI_DEG2RAD(mPitch));
	mFront.x = cos(MAI_DEG2RAD(mYaw)) * cos(MAI_DEG2RAD(mPitch));
	mFront.z = sin(MAI_DEG2RAD(mYaw)) * cos(MAI_DEG2RAD(mPitch));

	mFront = mai::normalize(mFront);
}

void Camera::yaw(int xoffset) {
	mYaw += xoffset * mSensitivity;

	mFront.y = sin(MAI_DEG2RAD(mPitch));
	mFront.x = cos(MAI_DEG2RAD(mYaw)) * cos(MAI_DEG2RAD(mPitch));
	mFront.z = sin(MAI_DEG2RAD(mYaw)) * cos(MAI_DEG2RAD(mPitch));

	mFront = mai::normalize(mFront);
}

void Camera::update() {
	//将各个方向的可能进行汇总，得到一个最终移动方向
	mai::vec3f moveDirection = {0.0f, 0.0f, 0.0f};

	mai::vec3f front = mFront;
	mai::vec3f right = mai::normalize(cross(mFront, mTop));

	if (mMoveState & MOVE_FRONT) {
		moveDirection += front;
	}

	if (mMoveState & MOVE_BACK) {
		moveDirection += -front;
	}

	if (mMoveState & MOVE_LEFT) {
		moveDirection += -right;
	}

	if (mMoveState & MOVE_RIGHT) {
		moveDirection += right;
	}

	if (mai::length_squared(moveDirection) != 0) {
		moveDirection = mai::normalize(moveDirection);
		mPosition += mSpeed * moveDirection;
	}

	mViewMatrix = mai::look_at<float>(mPosition, mPosition + mFront, mTop);
}

