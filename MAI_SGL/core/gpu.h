#pragma once
#include <vector>

#include "../base.h"
#include "../data_structures.h"
#include "../pipeline/draw_pipeline.h"
#include "../resource/buffer_object.h"
#include "../resource/frame_buffer.h"
#include "../resource/texture.h"
#include "../resource/VAO.h"
#include "../shader/shader.h"
#include "render_state.h"

// Software Graphics Library
#define MAI_SGL mai::GPU::instance()

namespace mai
{

class GPU
{
public:
	static GPU* instance();
	GPU() = default;
	~GPU() noexcept;

	// Initialize the framebuffer surface.
	void init_surface(const uint32_t& width, const uint32_t& height, void* buffer = nullptr);

	// Clear color and depth buffers.
	void clear() noexcept;

	// Debug helper.
	void printVAO(const uint32_t& vaoID);

	uint32_t gen_buffer();
	void delete_buffer(const uint32_t& buffer_ID);
	void bind_buffer(const uint32_t& bufferType, const uint32_t& bufferID);
	void buffer_data(const uint32_t& bufferType, size_t dataSize, void* data);

	uint32_t gen_vertex_array();
	void delete_vertex_array(const uint32_t& VAO_ID);
	void bind_vertex_array(const uint32_t& vaoID);
	void vertex_attribute_pointer(
		const uint32_t& binding, const uint32_t& itemSize,
		const uint32_t& stride, const uint32_t& offset);

	uint32_t get_texture();
	void delete_texture(const uint32_t& tex_id);
	void bind_texture(const uint32_t& tex_id);
	void tex_image_2D(const uint32_t& width, const uint32_t& height, void* data);
	void tex_parameter(const uint32_t& param, const uint32_t& value);

	void use_program(Shader* shader);

	void enable(const uint32_t& value);
	void disable(const uint32_t& value);

	void draw_mod(const uint32_t& value);

	// Cull face
	void front_face(const uint32_t& value);
	void cull_face(const uint32_t& value);

	// Depth test
	void depth_function(const uint32_t& value);

	void draw_element(const uint32_t& drawMode, const uint32_t& first, const uint32_t& count);

private:
	static GPU* _instance;
	FrameBuffer* _frame_buffer = { nullptr };
	DrawPipeline _draw_pipeline;
	RenderState _render_state;

	// VBO/EBO state
	uint32_t _current_EBO = { 0 };
	uint32_t _current_VBO = { 0 };
	uint32_t _buffer_counter = { 0 };
	std::map<uint32_t, BufferObject*> _buffer_map;

	// VAO state
	uint32_t _VAO_counter = { 0 };
	uint32_t _current_VAO = { 0 };
	std::map<uint32_t, VertexArrayObject*> _VAO_map;

	Shader* _shader{ nullptr };
	mai::mat4f _screen_matrix;

	uint32_t _current_texture{ 0 };
	uint32_t _texture_counter{ 0 };
	std::map<uint32_t, Texture*> _texture_map;
};

}
