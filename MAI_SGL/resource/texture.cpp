#include "texture.h"
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
	RGBA resultColor;

	//处理uv坐标
	check_wrap(u, _wrap_U);
	check_wrap(v, _wrap_V);

	if (_filter == MAI_TEXTURE_FILTER_NEAREST)
	{
		int x = std::round(u * (_width - 1));
		int y = std::round(v * (_height - 1));

		int position = y * _width + x;
		resultColor = _buffer[position];
	}
	else if (_filter == MAI_TEXTURE_FILTER_LINEAR)
	{
		float x = u * static_cast<float>(_width - 1);
		float y = v * static_cast<float>(_height - 1);

		int left = std::floor(x);
		int right = std::ceil(x);
		int bottom = std::floor(y);
		int top = std::ceil(y);

		//对上下插值，得到左右
		float yScale = 0.0f;
		if (top == bottom)
			yScale = 1.0f;
		else
			yScale = (y - static_cast<float>(bottom)) / static_cast<float>(top - bottom);

		int positionLeftTop = top * _width + left;
		int positionLeftBottom = bottom * _width + left;
		int positionRightTop = top * _width + right;
		int positionRightBottom = bottom * _width + right;

		RGBA leftColor = mai::lerp(_buffer[positionLeftBottom], _buffer[positionLeftTop], yScale);
		RGBA rightColor = mai::lerp(_buffer[positionRightBottom], _buffer[positionRightTop], yScale);

		//对左右插值，得到结果
		float xScale = 0.0f;
		if (right == left)
			xScale = 1.0f;
		else 
			xScale = (x - static_cast<float>(left)) / static_cast<float>(right - left);

		resultColor = mai::lerp(leftColor, rightColor, xScale);
	}

	mai::vec4f result;
	result.x = static_cast<float>(resultColor._R) / 255.0;
	result.y = static_cast<float>(resultColor._G) / 255.0;
	result.z = static_cast<float>(resultColor._B) / 255.0;
	result.w = static_cast<float>(resultColor._A) / 255.0;

	return result;
}

void Texture::check_wrap(float& n, uint32_t type)
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

void Texture::set_parameter(uint32_t type, uint32_t value)
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
