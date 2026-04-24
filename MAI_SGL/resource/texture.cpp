#include "texture.h"
#include "../core/gpu.h"
#include "../math/math.h"

namespace mai {


Texture::Texture() {}
Texture::~Texture()
{
	if (_buffer != nullptr)
	{
		delete[] _buffer;
		_buffer = nullptr;
	}
}

void Texture::set_buffer_data(uint32_t width, uint32_t height, void* data)
{
	if (_buffer != nullptr)
	{
		delete[] _buffer;
		_buffer = nullptr;
	}

	_width = width;
	_height = height;

	if (_buffer == nullptr) {
		_buffer = new RGBA[_width * _height];
	}

	memcpy(_buffer, data, _width * _height * sizeof(RGBA));
}

mai::vec4f Texture::get_color(float u, float v)
{
	RGBA result_color;

	//处理uv坐标
	check_wrap(u, _wrap_U);
	check_wrap(v, _wrap_V);

	if (_filter == MAI_TEXTURE_FILTER_NEAREST)
	{
		MAI_SGL->add_texture_samples(1);
		int x = std::round(u * (_width - 1));
		int y = std::round(v * (_height - 1));

		int position = y * _width + x;
		result_color = _buffer[position];
	}
	else if (_filter == MAI_TEXTURE_FILTER_LINEAR)
	{
		MAI_SGL->add_texture_samples(4);
		float x = u * static_cast<float>(_width - 1);
		float y = v * static_cast<float>(_height - 1);

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

		int position_left_top = top * _width + left;
		int position_left_bottom = bottom * _width + left;
		int position_right_top = top * _width + right;
		int position_right_bottom = bottom * _width + right;

		RGBA left_color = mai::lerp(_buffer[position_left_bottom], _buffer[position_left_top], y_scale);
		RGBA right_color = mai::lerp(_buffer[position_right_bottom], _buffer[position_right_top], y_scale);

		//对左右插值，得到结果
		float x_scale = 0.0f;
		if (right == left)
			x_scale = 1.0f;
		else 
			x_scale = (x - static_cast<float>(left)) / static_cast<float>(right - left);

		result_color = mai::lerp(left_color, right_color, x_scale);
	}

	mai::vec4f result;
	result.x = static_cast<float>(result_color._R) / 255.0;
	result.y = static_cast<float>(result_color._G) / 255.0;
	result.z = static_cast<float>(result_color._B) / 255.0;
	result.w = static_cast<float>(result_color._A) / 255.0;

	return result;
}

void Texture::check_wrap(float& n, uint8_t type)
{
	if (n > 1.0f || n < 0.0f)
	{
		n = MAI_FRACTION(n);

		switch (type)
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

void Texture::set_parameter(uint8_t type, uint32_t value)
{
	switch (type)
	{
	case MAI_TEXTURE_FILTER:
		_filter = value;
		break;
	case MAI_TEXTURE_WRAP_U:
		_wrap_U = value;
		break;
	case MAI_TEXTURE_WRAP_V:
		_wrap_V = value;
		break;
	default:
		break;
	}
}

}
