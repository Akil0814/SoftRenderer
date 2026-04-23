#include"buffer_object.h"

namespace mai
{

BufferObject::BufferObject() {}

BufferObject::~BufferObject()
{
	if (_buffer)
		delete[] _buffer;
}

void BufferObject::set_buffer_data(size_t data_size, void* data)
{
	if (_buffer != nullptr && _buffer_size < data_size)
	{
		delete[] _buffer;
		_buffer = nullptr;
	}

	if (_buffer == nullptr)
	{
		_buffer = new byte[data_size];
		_buffer_size = data_size;
	}

	memcpy(_buffer, data, data_size);
}

byte* BufferObject::get_buffer() const
{
	return _buffer;
}

size_t BufferObject::get_size() const
{
	return _buffer_size;
}

}
