#pragma once
#include "../global/base.h"

namespace mai
{

	class BufferObject
	{
	public:
		BufferObject();
		~BufferObject();
		BufferObject(const BufferObject&) = delete;

		//只有调用本函数的时候，才进行内存空间的分配
		void set_buffer_data(size_t dataSize, void* data);
		byte* get_buffer() const;
		size_t get_size() const;

	private:
		size_t	_buffer_size = { 0 };//bytes
		byte* _buffer = { nullptr };
	};

}