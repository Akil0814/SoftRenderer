#include "gpu.h"
#include "raster.h"

namespace mai
{

GPU* GPU::_instance = nullptr;

GPU* GPU::instance()
{
	if (!_instance)
		_instance = new GPU();

	return _instance;
}

GPU::~GPU()
{
	if (_frame_buffer)
		delete _frame_buffer;
}

void GPU::init_surface(const uint32_t& width, const uint32_t& height, void* buffer)
{
	_frame_buffer = new FrameBuffer(width, height, buffer);
}

void GPU::clear()
{
	size_t pixelSize = _frame_buffer->_width * _frame_buffer->_height;
	std::fill_n(_frame_buffer->_color_buffer, pixelSize, RGBA(0, 0, 0, 0));
}

void GPU::draw_point(const uint32_t& x, const uint32_t& y, const RGBA& color)
{
	//从窗口左下角开始算起
	uint32_t pixelPos = y * _frame_buffer->_width + x;
	_frame_buffer->_color_buffer[pixelPos] = color;
}

void GPU::draw_line(const Point& p1, const Point& p2)
{
	std::vector<Point> pixels;
	Raster::rasterize_line(p1, p2, pixels);

	for (const auto& p : pixels)
	{
		draw_point(p.x, p.y, p.color);
	}
}

void GPU::draw_triangle(const Point& p1, const Point& p2, const Point& p3)
{
	std::vector<Point> pixels;
	Raster::rasterize_triangle(p1, p2, p3, pixels);

	for (const auto& p : pixels)
	{
		draw_point(p.x, p.y, p.color);
	}
}



}
