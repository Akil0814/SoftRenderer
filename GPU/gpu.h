#pragma once
#include "../application/application.h"
#include "../application/image.h"

#include "../global/base.h"

#include "data_structures.h"
#include "frame_buffer.h"
#include "buffer_object.h"
#include "VAO.h"
#include "shader.h"
#include "texture.h"


//Software Graphics Library
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

	uint32_t get_texture();
	void delete_texture(const uint32_t& tex_id);
	void bind_texture(const uint32_t& tex_id);
	void tex_image_2D(const uint32_t& width,const uint32_t& height, void* data);
	void tex_parameter(const uint32_t& param, const uint32_t& value);

	void use_program(Shader* shader);

	void enable(const uint32_t& value);
	void disable(const uint32_t& value);

	//cull face
	void front_face(const uint32_t& value);
	void cull_face(const uint32_t& value);

	//depth test
	void depth_function(const uint32_t& value);

	void draw_element(const uint32_t& drawMode, const uint32_t& first, const uint32_t& count);


private:
	void vertex_shader_stage(
		const VertexArrayObject* vao, const BufferObject* ebo,
		const uint32_t first, const uint32_t count,
		std::vector<VsOutput>& vsOutputs);

	void perspective_division(VsOutput& vsOutput);
	void perspective_recover(VsOutput& vsOutput);
	void screen_mapping(VsOutput& vsOutput);

	void trim(VsOutput& vsOutput);

	bool depth_test(const FsOutput& fsOutput);

	RGBA blend(const FsOutput& output);

private:
	static GPU* _instance;
	FrameBuffer* _frame_buffer = { nullptr };

	//VBO相关/EBO也存在内部
	uint32_t _current_EBO = { 0 };
	uint32_t _current_VBO = { 0 };
	uint32_t _buffer_counter={ 0 };
	std::map<uint32_t, BufferObject*> _buffer_map;

	//VAO相关
	uint32_t _VAO_counter = { 0 };
	uint32_t _current_VAO = { 0 };
	std::map<uint32_t, VertexArrayObject*> _VAO_map;

	Shader* _shader { nullptr };
	mai::mat4f _screen_matrix;

	bool _enable_cull_face{ true };
	uint32_t _front_face{ MAI_FRONT_FACE_CCW };
	uint32_t _cull_face{ MAI_BACK_FACE };

	bool _enable_depth_test{ true };
	uint32_t _depth_function{ MAI_DEPTH_LESS };

	bool _enable_blending{ true };

	uint32_t _current_texture{ 0 };
	uint32_t _texture_counter{ 0 };
	std::map<uint32_t, Texture*> _texture_map;
};

}
