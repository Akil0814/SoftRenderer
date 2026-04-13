#include"buffer_object.h"

namespace mai
{

BufferObject::BufferObject() {}

BufferObject::~BufferObject()
{
	if (_buffer)
		delete[] _buffer;
}

void BufferObject::set_buffer_data(size_t dataSize, void* data)
{
	if (_buffer != nullptr && _buffer_size < dataSize)
	{
		delete[] _buffer;
		_buffer = nullptr;
	}

	if (_buffer == nullptr)
	{
		_buffer = new byte[dataSize];
		_buffer_size = dataSize;
	}

	memcpy(_buffer, data, dataSize);
}

byte* BufferObject::get_buffer() const
{
	return _buffer;
}

}