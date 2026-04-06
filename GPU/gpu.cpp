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

inline void GPU::draw_point(const uint32_t& x, const uint32_t& y, const RGBA& color)
{
	if (x >= _frame_buffer->_width || y >= _frame_buffer->_height)
		return;

	//从窗口左下角开始算起
	uint32_t pixel_pos = y * _frame_buffer->_width + x;

	if (!_enable_blending)
	{
		_frame_buffer->_color_buffer[pixel_pos] = color;
		return;
	}

	const RGBA& src = color;

	if (src._A == 255)
	{
		_frame_buffer->_color_buffer[pixel_pos] = src;
		return;
	}

	if (src._A == 0)
		return;

	RGBA result = color;
	const RGBA& dst = _frame_buffer->_color_buffer[pixel_pos];


	//颜色混合
	float weight = static_cast<float>(src._A) / 255.0f;

	result._R = static_cast<float>(src._R) * weight + static_cast<float>(dst._R) * (1.0f - weight);
	result._G = static_cast<float>(src._G) * weight + static_cast<float>(dst._G) * (1.0f - weight);
	result._B = static_cast<float>(src._B) * weight + static_cast<float>(dst._B) * (1.0f - weight);
	result._A = static_cast<float>(src._A) * weight + static_cast<float>(dst._A) * (1.0f - weight);

	_frame_buffer->_color_buffer[pixel_pos] = result;
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

void GPU::draw_image(const Image* image)
{
	if (image == nullptr || image->_data == nullptr)
		return;

	for (uint32_t y = 0; y < image->_height; ++y)
	{
		for (uint32_t x = 0; x < image->_width; ++x)
		{
			draw_point(x, y, image->_data[y * image->_width + x]);
		}
	}
}

void GPU::draw_image_with_alpha(const Image* image, const uint32_t& alpha)
{
	if (image == nullptr || image->_data == nullptr)
		return;

	for (uint32_t y = 0; y < image->_height; ++y)
	{
		for (uint32_t x = 0; x < image->_width; ++x)
		{
			RGBA color = image->_data[y * image->_width + x];
			color._A = static_cast<byte>(
				static_cast<uint32_t>(color._A) * alpha / 255
				);
			draw_point(x, y, color);
		}
	}
}

void GPU::set_blending(bool enable)
{
	_enable_blending = enable;
}


}
