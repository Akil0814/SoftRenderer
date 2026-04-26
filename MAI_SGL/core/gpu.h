#pragma once
#include <fstream>
#include <string>
#include <vector>

#include "../base.h"
#include "../data_structures.h"
#include "../pipeline/draw_pipeline.h"
#include "../resource/buffer_object.h"
#include "../resource/frame_buffer.h"
#include "../resource/texture.h"
#include "../resource/VAO.h"
#include "../shader/shader.h"
#include "gpu_debug.h"
#include "gpu_stats.h"
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

	void clear() noexcept;

	void draw_element(uint8_t draw_mode, size_t first, size_t count);

	uint32_t gen_buffer();
	void delete_buffer(uint32_t buffer_ID);
	void bind_buffer(uint8_t buffer_type, uint32_t buffer_ID);
	void buffer_data(uint8_t buffer_type, size_t data_size, void* data);

	uint32_t gen_vertex_array();
	void delete_vertex_array(uint32_t VAO_ID);
	void bind_vertex_array(uint32_t VAO_ID);
	void vertex_attribute_pointer(uint32_t binding, size_t item_size,size_t stride, size_t offset);

	uint32_t gen_texture();
	void delete_texture(uint32_t tex_ID);
	void bind_texture(uint32_t tex_ID);
	void tex_image_2D(uint32_t width, uint32_t height, void* data);
	void tex_parameter(uint8_t param, uint32_t value);

	void enable(uint8_t value);
	void disable(uint8_t value);

	void use_program(Shader* shader);
	void draw_dimension(uint8_t value);
	void set_scissor_rect(const ScissorRect& rect);
	void front_face(uint8_t value);
	void cull_face(uint8_t value);
	void depth_function(uint8_t value);


	Shader* get_program() const;
	RenderState get_render_state() const;
	void set_render_state(const RenderState& state);
	uint32_t get_bound_vertex_array() const;
	uint32_t get_bound_array_buffer() const;
	uint32_t get_bound_element_array_buffer() const;
	RenderStats get_render_stats() const;
	RenderStats get_summary_stats() const;
	void add_rasterized_pixels(uint64_t count) noexcept;
	void add_fragments(uint64_t count) noexcept;
	void add_texture_samples(uint64_t count) noexcept;
	gpu_debug::ErrorCode get_error() noexcept;
	gpu_debug::ErrorCode peek_error() const noexcept;


	// Debug-only.
	void print_VAO(uint32_t VAO_ID);
	bool set_stats_output_path(const std::string& output_path_prefix);
	void print_frame_stats();
	void print_summary_stats();


private:
	void reset_render_stats() noexcept;
	void accumulate_draw_stats(uint8_t draw_mode, size_t count) noexcept;
	void accumulate_pipeline_stats(const RenderStats& stats) noexcept;
	void set_error(gpu_debug::ErrorCode error) noexcept;
	void write_frame_stats_csv();
	void write_summary_stats_txt();

	static GPU* _instance;
	FrameBuffer* _frame_buffer = { nullptr };
	DrawPipeline _draw_pipeline;
	RenderState _render_state;
	RenderStats _render_stats;
	RenderStats _summary_render_stats;
	gpu_debug::ErrorCode _error_code{ gpu_debug::ErrorCode::NoError };
	std::ofstream _frame_stats_file;
	std::ofstream _summary_stats_file;
	uint64_t _stats_output_frame_index{ 0 };

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
