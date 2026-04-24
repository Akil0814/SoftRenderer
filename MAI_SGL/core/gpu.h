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
	void init_surface(uint32_t width, uint32_t height, void* buffer = nullptr);

	// Clear color and depth buffers.
	void clear() noexcept;

	// Debug helper.
	void print_VAO(uint32_t VAO_ID);

	uint32_t gen_buffer();
	void delete_buffer(uint32_t buffer_ID);
	void bind_buffer(uint8_t buffer_type, uint32_t buffer_ID);
	void buffer_data(uint8_t buffer_type, size_t data_size, void* data);

	uint32_t gen_vertex_array();
	void delete_vertex_array(uint32_t VAO_ID);
	void bind_vertex_array(uint32_t VAO_ID);
	void vertex_attribute_pointer(
		uint32_t binding, size_t item_size,
		size_t stride, size_t offset);

	uint32_t get_texture();
	void delete_texture(uint32_t tex_ID);
	void bind_texture(uint32_t tex_ID);
	void tex_image_2D(uint32_t width, uint32_t height, void* data);
	void tex_parameter(uint8_t param, uint32_t value);

	void use_program(Shader* shader);
	Shader* get_program() const;

	void enable(uint8_t value);
	void disable(uint8_t value);

	void draw_dimension(uint8_t value);

	void set_scissor_rect(const ScissorRect& rect);

	// Cull face
	void front_face(uint8_t value);
	void cull_face(uint8_t value);

	// Depth test
	void depth_function(uint8_t value);


	RenderState get_render_state() const;
	void set_render_state(const RenderState& state);
	uint32_t get_bound_vertex_array() const;
	uint32_t get_bound_array_buffer() const;
	uint32_t get_bound_element_array_buffer() const;

	void draw_element(uint8_t draw_mode, size_t first, size_t count);

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
