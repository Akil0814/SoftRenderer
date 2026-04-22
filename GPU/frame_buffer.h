#pragma once
#include "../global/base.h"

namespace mai
{

/*
* class FrameBuffer：
* 存储当前画布对应的bmp的内存指针，作为当前绘图画板
*/
class FrameBuffer
{
public:
	FrameBuffer(uint32_t width, uint32_t height, void* buffer = nullptr);
	~FrameBuffer() noexcept;
	FrameBuffer(const FrameBuffer&) = delete;

	uint32_t _width = { 0 };
	uint32_t _height = { 0 };
	RGBA* _color_buffer = { nullptr }; //真正指向像素数据
	float* _depth_buffer = { nullptr };
	bool _extern_buffer = { false }; //表示这块内存是不是外部的
};
}
