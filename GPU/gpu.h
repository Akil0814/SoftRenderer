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

	uint32_t gen_buffer();
	void delete_buffer(const uint32_t& buffer_ID);

	uint32_t gen_vertex_array();
	void delete_vertex_array(const uint32_t& VAO_ID);


private:
	static GPU* _instance;
	FrameBuffer* _frame_buffer = { nullptr };

	//VBO相关/EBO也存在内部
	uint32_t _buffer_counter{ 0 };
	std::map<uint32_t, BufferObject*> _buffer_map;

	//VAO相关
	uint32_t _VAO_counter{ 0 };
	std::map<uint32_t, VertexArrayObject*> _VAO_map;

};

}
