#pragma once
#include "../global/base.h"
#include "data_structures.h"
#include "frame_buffer.h"
#include "buffer_object.h"
#include "VAO.h"

#include "../application/application.h"
#include "../application/image.h"

#define MAI_SGL mai::GPU::instance()

namespace mai
{

class GPU
{
public:
	static GPU* instance();
	GPU() = default;
	~GPU() noexcept;

	//接受外界传入的bmp对应的内存指针以及窗体的宽/高
	void init_surface(const uint32_t& width, const uint32_t& height, void* buffer = nullptr);

	//清除画布内容
	void clear() noexcept;

	//打印状态机
	void printVAO(const uint32_t& vaoID);

	uint32_t gen_buffer();
	void delete_buffer(const uint32_t& buffer_ID);
	void bind_buffer(const uint32_t& bufferType, const uint32_t& bufferID);
	void buffer_data(const uint32_t& bufferType, size_t dataSize, void* data);

	uint32_t gen_vertex_array();
	void delete_vertex_array(const uint32_t& VAO_ID);

	void bind_vertex_array(const uint32_t& vaoID);
	void vertex_attribute_pointer(
		const uint32_t& binding,const uint32_t& itemSize,
		const uint32_t& stride,const uint32_t& offset);


private:
	static GPU* _instance;
	FrameBuffer* _frame_buffer = { nullptr };

	//VBO相关/EBO也存在内部
	uint32_t _current_EBO = { 0 };
	uint32_t _current_VBO = { 0 };
	uint32_t _buffer_counter={ 0 };
	std::map<uint32_t, BufferObject*> _buffer_map;

	//VAO相关
	uint32_t _VAO_counter={ 0 };
	uint32_t _current_VAO={ 0 };
	std::map<uint32_t, VertexArrayObject*> _VAO_map;

};

}
