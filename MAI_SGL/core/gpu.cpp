#include "gpu.h"

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

	void GPU::init_surface(uint32_t width, uint32_t height, void* buffer)
	{
		_frame_buffer = new FrameBuffer(width, height, buffer);
		_screen_matrix = screen_matrix<float>(width - 1, height - 1);
	}

	void GPU::clear() noexcept
	{
		reset_render_stats();
		size_t pixelSize = _frame_buffer->_width * _frame_buffer->_height;
		std::fill_n(_frame_buffer->_color_buffer, pixelSize, RGBA(0, 0, 0, 0));
		std::fill_n(_frame_buffer->_depth_buffer, pixelSize, 1.0f);
	}

	void GPU::print_VAO(uint32_t VAO_ID)
	{
		auto iter = _VAO_map.find(VAO_ID);
		if (iter != _VAO_map.end())
			iter->second->print();
	}

	uint32_t GPU::gen_buffer()
	{
		++_buffer_counter;
		_buffer_map.insert(std::make_pair(_buffer_counter, new BufferObject()));

		return _buffer_counter;
	}

	void GPU::delete_buffer(uint32_t buffer_ID)
	{
		auto iter = _buffer_map.find(buffer_ID);

		if (iter != _buffer_map.end())
			delete iter->second;
		else
			return;

		_buffer_map.erase(iter);
	}

	void GPU::bind_buffer(uint8_t buffer_type, uint32_t buffer_ID)
	{
		if (buffer_type == MAI_ARRAY_BUFFER)
			_current_VBO = buffer_ID;

		else if (buffer_type == MAI_ELEMENT_ARRAY_BUFFER)
			_current_EBO = buffer_ID;
	}

	void GPU::buffer_data(uint8_t buffer_type, size_t data_size, void* data)
	{
		uint32_t buffer_ID = 0;

		if (buffer_type == MAI_ARRAY_BUFFER)
			buffer_ID = _current_VBO;
		else if (buffer_type == MAI_ELEMENT_ARRAY_BUFFER)
			buffer_ID = _current_EBO;
		else
			assert(false);

		auto iter = _buffer_map.find(buffer_ID);
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

	void GPU::delete_vertex_array(uint32_t VAO_ID)
	{
		auto iter = _VAO_map.find(VAO_ID);

		if (iter != _VAO_map.end())
			delete iter->second;
		else
			return;

		_VAO_map.erase(iter);
	}

	void GPU::bind_vertex_array(uint32_t VAO_ID)
	{
		_current_VAO = VAO_ID;
	}

	void GPU::vertex_attribute_pointer(
		uint32_t binding, size_t item_size,
		size_t stride, size_t offset)
	{
		auto iter = _VAO_map.find(_current_VAO);
		if (iter == _VAO_map.end())
			assert(false);

		auto VBO_iter = _buffer_map.find(_current_VBO);
		if (VBO_iter == _buffer_map.end())
			assert(false);

		auto vao = iter->second;
		vao->set(binding, _current_VBO, item_size, stride, offset);
	}

	void GPU::use_program(Shader* shader)
	{
		_shader = shader;
	}

	Shader* GPU::get_program() const
	{
		return _shader;
	}

	void GPU::draw_element(uint8_t draw_mode, size_t first, size_t count)
	{
		if (_current_VAO == 0 || _shader == nullptr || count == 0)
			return;

		auto VAO_iter = _VAO_map.find(_current_VAO);

		if (VAO_iter == _VAO_map.end())
		{
			std::cerr << "Error: current vao is invalid!" << std::endl;
			return;
		}

		const VertexArrayObject* vao = VAO_iter->second;

		auto EBO_iter = _buffer_map.find(_current_EBO);

		if (EBO_iter == _buffer_map.end())
		{
			std::cerr << "Error: current ebo is invalid!" << std::endl;
			return;
		}

		const BufferObject* ebo = EBO_iter->second;

		DrawContext context{
			*_frame_buffer,
			*_shader,
			_render_state,
			_buffer_map,
			_texture_map,
			*vao,
			*ebo,
			_screen_matrix
		};

		accumulate_draw_stats(draw_mode, count);
		_draw_pipeline.draw_elements(context, draw_mode, first, count);
	}

	void GPU::enable(uint8_t value)
	{
		switch (value)
		{
		case MAI_CULL_FACE:
			_render_state._enable_cull_face = true;
			break;
		case MAI_DEPTH_TEST:
			_render_state._enable_depth_test = true;
			break;
		case MAI_BLENDING:
			_render_state._enable_blending = true;
			break;
		case MAI_SCISSOR_TEST:
			_render_state._enable_scissor_test=true;
			break;		
		default:
			break;
		}
	}

	void GPU::disable(uint8_t value)
	{
		switch (value)
		{
		case MAI_CULL_FACE:
			_render_state._enable_cull_face = false;
			break;
		case MAI_DEPTH_TEST:
			_render_state._enable_depth_test = false;
			break;
		case MAI_BLENDING:
			_render_state._enable_blending = false;
			break;
		case MAI_SCISSOR_TEST:
			_render_state._enable_scissor_test = false;
			break;	
		default:
			break;
		}
	}

	void GPU::front_face(uint8_t value)
	{
		_render_state._front_face = static_cast<uint8_t>(value);
	}

	void GPU::cull_face(uint8_t value)
	{
		_render_state._cull_face = static_cast<uint8_t>(value);
	}

	void GPU::depth_function(uint8_t value)
	{
		_render_state._depth_function = static_cast<uint8_t>(value);
	}

	uint32_t GPU::get_texture()
	{
		_texture_counter++;
		_texture_map.insert(std::make_pair(_texture_counter, new Texture()));

		return _texture_counter;
	}

	void GPU::delete_texture(uint32_t tex_ID)
	{
		auto iter = _texture_map.find(tex_ID);
		if (iter != _texture_map.end())
			delete iter->second;
		else
			return;

		_texture_map.erase(iter);
	}

	void GPU::bind_texture(uint32_t tex_ID)
	{
		_current_texture = tex_ID;
	}

	void GPU::tex_image_2D(uint32_t width, uint32_t height, void* data)
	{
		if (!_current_texture)
			return;

		auto iter = _texture_map.find(_current_texture);
		if (iter == _texture_map.end()) {
			return;
		}
		auto texture = iter->second;
		texture->set_buffer_data(width, height, data);
	}

	void GPU::tex_parameter(uint8_t param, uint32_t value)
	{
		if (!_current_texture)
			return;

		auto iter = _texture_map.find(_current_texture);
		if (iter == _texture_map.end())
			return;

		Texture* texture = iter->second;
		texture->set_parameter(param, value);
	}

	void GPU::draw_dimension(uint8_t value)
	{
		_render_state._draw_dimension = value;
	}

	void GPU::set_scissor_rect(const ScissorRect& rect)
	{
		_render_state._scissor_clip_rect = rect;
	}

	RenderState  GPU::get_render_state() const
	{
		return _render_state;
	}

	void  GPU::set_render_state(const RenderState& state)
	{
		_render_state = state;
	}

	uint32_t GPU::get_bound_vertex_array() const
	{
		return _current_VAO;
	}

	uint32_t GPU::get_bound_array_buffer() const
	{
		return _current_VBO;
	}

	uint32_t GPU::get_bound_element_array_buffer() const
	{
		return _current_EBO;
	}

	RenderStats GPU::get_render_stats() const
	{
		return _render_stats;
	}

	void GPU::reset_render_stats() noexcept
	{
		_render_stats = RenderStats{};
	}

	void GPU::accumulate_draw_stats(uint8_t draw_mode, size_t count) noexcept
	{
		++_render_stats._frame_draw_calls;
		_render_stats._frame_vertices += count;

		if (draw_mode == MAI_DRAW_TRIANGLES)
			_render_stats._frame_triangles += count / 3;
	}
}
