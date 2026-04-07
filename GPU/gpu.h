#pragma once
#include "../global/base.h"
#include "frame_buffer.h"

#include "../application/application.h"
#include "../application/image.h"

#define MAI_SGL mai::GPU::instance()

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

	void draw_triangle(const Point& p1, const Point& p2,const Point& p3);

	void draw_image(const Image* image);

	void draw_image_with_alpha(const Image* image, const uint32_t& alpha);

	//设置状态
	void set_blending(bool enable);

	void set_bilinear(bool enable);

	void set_texture(Image* image);

	void set_texture_wrap(uint32_t wrap);

private:

	RGBA sample_nearest(const mai::vec2f& uv);
	RGBA sample_bilinear(const mai::vec2f& uv);
	void check_wrap(float& n);

private:
	static GPU* _instance;

	bool _enable_blending = { false };
	bool _enable_bilinear = { false };

	uint32_t _wrap = { MAI_TEXTURE_WRAP_REPEAT };

	FrameBuffer* _frame_buffer = { nullptr };

	Image* _image = { nullptr };
};

}
