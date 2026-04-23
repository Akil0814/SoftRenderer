#pragma once
#include "../base.h"
#include "../math/math.h"

namespace mai {

class Texture
{
public:
	Texture();
	~Texture();

	//默认都是rgba格式
	void set_buffer_data(const uint32_t& width, const uint32_t& height, void* data);
	mai::vec4f get_color(float u, float v);
	void set_parameter(const uint32_t& type, const uint32_t& value);

private:
	void check_wrap(float& n, const uint32_t& type);

private:
	RGBA* _buffer{ nullptr };
	uint32_t _width{ 0 };
	uint32_t _height{ 0 };

	uint32_t _filter{ MAI_TEXTURE_FILTER_NEAREST };
	uint32_t _wrap_U{ MAI_TEXTURE_WRAP_REPEAT };
	uint32_t _wrap_V{ MAI_TEXTURE_WRAP_REPEAT };
};

}
