#include "frame_buffer.h"

namespace mai
{

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, void* buffer)
{
	_width = width;
	_height = height;

	if (!buffer)
	{
		buffer = new RGBA[width * height];
		_extern_buffer = false;
	}
	else
		_extern_buffer = true;

	_color_buffer = (RGBA*)buffer;

	_depth_buffer = new float[width * height];
	std::fill_n(_depth_buffer, width * height, 1.0f);
}

FrameBuffer::~FrameBuffer() noexcept
{
	if (!_extern_buffer && _color_buffer)
		delete[] _color_buffer;

	if (_depth_buffer)
		delete[] _depth_buffer;
}

}
