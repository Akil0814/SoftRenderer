#pragma once
#include "../base.h"

namespace mai
{
	class Image
	{
	public:
		Image(uint32_t width = 0, uint32_t height = 0, RGBA* data = nullptr);
		~Image();
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;

		static Image* create_image(const std::string& path);
		static void destroy_image(Image* image);

	public:
		uint32_t _width{ 0 };
		uint32_t _height{ 0 };
		RGBA* _data{ nullptr };
	};
}
