#include "gpu.h"
#include "raster.h"
#include <algorithm>

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

	RGBA result_color;
	for (const auto& p : pixels)
	{
		if (_image)
			result_color = _enable_bilinear ? sample_bilinear(p.uv) : sample_nearest(p.uv);
		else
			result_color = p.color;

		draw_point(p.x, p.y, result_color);
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

RGBA GPU::sample_nearest(const mai::vec2f& uv)
{
	mai::vec2f f_uv = uv;

	check_wrap(f_uv.x);
	check_wrap(f_uv.y);

	//四舍五入到最近整数
	// u = 0 对应 x = 0，u = 1 对应 x = width - 1
	// v = 0 对应 y = 0，v = 1 对应 y = height - 1
	int x = std::round(f_uv.x * (_image->_width - 1));
	int y = std::round(f_uv.y * (_image->_height - 1));
	x = std::clamp(x, 0, static_cast<int>(_image->_width) - 1);
	y = std::clamp(y, 0, static_cast<int>(_image->_height) - 1);

	int position = y * _image->_width + x;

	return _image->_data[position];
}

RGBA GPU::sample_bilinear(const mai::vec2f& uv)
{
	RGBA result_color;
	mai::vec2f f_uv = uv;

	check_wrap(f_uv.x);
	check_wrap(f_uv.y);

	float x = f_uv.x * static_cast<float>(_image->_width - 1);
	float y = f_uv.y * static_cast<float>(_image->_height - 1);


	//获取目标点周围四个点的坐标
	int left = std::floor(x);
	int right = std::ceil(x);
	int bottom = std::floor(y);
	int top = std::ceil(y);

	//对上下插值，得到左右
	float y_scale = 0.0f;
	if (top == bottom)
		y_scale = 1.0f;
	else
		y_scale = (y - static_cast<float>(bottom)) / static_cast<float>(top - bottom);

	int position_left_top = top * _image->_width + left;
	int position_left_bottom = bottom * _image->_width + left;
	int position_right_top = top * _image->_width + right;
	int position_right_bottom = bottom * _image->_width + right;

	RGBA left_color = Raster::lerpRGBA(_image->_data[position_left_bottom], _image->_data[position_left_top], y_scale);
	RGBA right_color = Raster::lerpRGBA(_image->_data[position_right_bottom], _image->_data[position_right_top], y_scale);

	//对左右插值，得到结果
	float x_scale = 0.0f;
	if (right == left)
		x_scale = 1.0f;
	else
		x_scale = (x - static_cast<float>(left)) / static_cast<float>(right - left);

	result_color = Raster::lerpRGBA(left_color, right_color, x_scale);

	return result_color;
}


void GPU::set_blending(bool enable)
{
	_enable_blending = enable;
}

void GPU::set_bilinear(bool enable)
{
	_enable_bilinear = enable;
}

void GPU::set_texture_wrap(uint32_t wrap)
{
	_wrap = wrap;
}

void GPU::check_wrap(float& n)
{
	if (n > 1.0f || n < 0.0f)
	{
		n = MAI_FRACTION(n);
		switch (_wrap)
		{
		case MAI_TEXTURE_WRAP_REPEAT:
			n = MAI_FRACTION(n + 1);
			break;

		case MAI_TEXTURE_WRAP_MIRROR:
			n = 1.0f - MAI_FRACTION(n + 1);
			break;

		default:
			break;
		}
	}
}


void GPU::set_texture(Image* image)
{
	_image = image;
}

}
