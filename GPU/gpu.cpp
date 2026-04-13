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

GPU::~GPU() noexcept
{
	if (_frame_buffer)
		delete _frame_buffer;

	for (auto iter : _buffer_map)
	{
		delete iter.second;
	}
	_buffer_map.clear();

	for (auto iter : _VAO_map)
	{
		delete iter.second;
	}
	_VAO_map.clear();

}

void GPU::init_surface(const uint32_t& width, const uint32_t& height, void* buffer)
{
	_frame_buffer = new FrameBuffer(width, height, buffer);
}

void GPU::clear() noexcept
{
	size_t pixelSize = _frame_buffer->_width * _frame_buffer->_height;
	std::fill_n(_frame_buffer->_color_buffer, pixelSize, RGBA(0, 0, 0, 0));
}

uint32_t GPU::gen_buffer()
{
	++_buffer_counter;
	_buffer_map.insert(std::make_pair(_buffer_counter, new BufferObject()));

	return _buffer_counter;
}

void GPU::delete_buffer(const uint32_t& buffer_ID)
{
	auto iter = _buffer_map.find(buffer_ID);

	if (iter != _buffer_map.end())
		delete iter->second;
	else
		return;

	_buffer_map.erase(iter);
}

uint32_t GPU::gen_vertex_array()
{
	_VAO_counter++;
	_VAO_map.insert(std::make_pair(_VAO_counter, new VertexArrayObject()));

	return _VAO_counter;
}

void GPU::delete_vertex_array(const uint32_t& VAO_ID)
{
	auto iter = _VAO_map.find(VAO_ID);

	if (iter != _VAO_map.end())
		delete iter->second;
	else 
		return;

	_VAO_map.erase(iter);
}


}
