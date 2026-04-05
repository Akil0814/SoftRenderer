#pragma once
#include "../global/base.h"
#include "frame_buffer.h"
#include "../application/application.h"

#define sgl mai::GPU::instance()

namespace mai
{

class GPU
{
public:
	static GPU* instance();
	GPU() = default;
	~GPU();

	//接受外界传入的bmp对应的内存指针以及窗体的宽/高
	void init_surface(const uint32_t& width, const uint32_t& height, void* buffer = nullptr);

	//清除画布内容
	void clear();

	//传入像素位置，绘制成某种颜色
	void draw_point(const uint32_t& x, const uint32_t& y, const RGBA& color);

	void draw_line(const Point& p1, const Point& p2);

private:
	static GPU* _instance;

	FrameBuffer* _frame_buffer = { nullptr };
};

}
