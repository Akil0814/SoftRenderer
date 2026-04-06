#define STB_IMAGE_IMPLEMENTATION
#include"../thirdParty/stb_image.h"
#include "image.h"

namespace mai
{

	Image::Image(const uint32_t& width, const uint32_t& height, RGBA* data)
		:_width(width),_height(height), _data(nullptr)
	{
		if (data)
		{
			_data = new RGBA[_width * _height];
			memcpy(_data, data, sizeof(RGBA) * _width * _height);
		}
	}
	Image::~Image()
	{
		if (_data != nullptr)
			delete[] _data;
	}

	Image* Image::create_image(const std::string& path)
	{
		int pic_type = 0;
		int width = 0, height = 0;

		stbi_set_flip_vertically_on_load(true);

		unsigned char* bits = stbi_load(path.c_str(), &width, &height, &pic_type, STBI_rgb_alpha);
		if (!bits)
			return nullptr;

		//win GDI用的是BGRA 图片是RGBA 需要交换R&B
		for (int i = 0; i < width * height * 4; i += 4)
		{
			std::swap(bits[i], bits[i + 2]);
		}

		Image* image = new Image(width, height, (RGBA*)bits);

		stbi_image_free(bits);

		return image;
	}

	void Image::destroy_image(Image* image)
	{
		if (image)
			delete image;
	}

}