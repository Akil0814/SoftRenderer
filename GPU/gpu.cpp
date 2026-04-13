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

void GPU::printVAO(const uint32_t& vaoID)
{
	auto iter = _VAO_map.find(vaoID);
	if (iter != _VAO_map.end())
		iter->second->print();
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

void GPU::bind_buffer(const uint32_t& buffer_type, const uint32_t& bufferID)
{
	if (buffer_type == MAI_ARRAY_BUFFER)
		_current_VBO = bufferID;

	else if (buffer_type == MAI_ELEMENT_ARRAY_BUFFER)
		_current_EBO = bufferID;
}

void GPU::buffer_data(const uint32_t& buffer_type, size_t data_size, void* data)
{
	uint32_t bufferID = 0;

	if (buffer_type == MAI_ARRAY_BUFFER)
		bufferID = _current_VBO;
	else if (buffer_type == MAI_ELEMENT_ARRAY_BUFFER)
		bufferID = _current_EBO;
	else
		assert(false);

	auto iter = _buffer_map.find(bufferID);
	if (iter == _buffer_map.end())
		assert(false);

	BufferObject* buffer_object = iter->second;
	buffer_object->set_buffer_data(data_size, data);
}

uint32_t GPU::gen_vertex_array()
{
	++_VAO_counter;
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

void GPU::bind_vertex_array(const uint32_t& VAO_ID)
{
	_current_VAO = VAO_ID;
}

void GPU::vertex_attribute_pointer(
	const uint32_t& binding,const uint32_t& item_size,
	const uint32_t& stride,const uint32_t& offset)
{
	auto iter = _VAO_map.find(_current_VAO);
	if (iter == _VAO_map.end())
		assert(false);

	auto vboIter = _buffer_map.find(_current_VBO);
	if (vboIter == _buffer_map.end())
		assert(false);

	auto vao = iter->second;
	vao->set(binding, _current_VBO, item_size, stride, offset);
}

}
