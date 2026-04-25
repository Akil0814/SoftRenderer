#include "gpu.h"

#include <algorithm>
#include <new>

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
		if (width == 0 || height == 0)
		{
			set_error(gpu_debug::ErrorCode::InvalidValue);
			return;
		}

		if (_frame_buffer)
			delete _frame_buffer;

		try
		{
			_frame_buffer = new FrameBuffer(width, height, buffer);
		}
		catch (const std::bad_alloc&)
		{
			_frame_buffer = nullptr;
			set_error(gpu_debug::ErrorCode::OutOfMemory);
			return;
		}

		_screen_matrix = screen_matrix<float>(width - 1, height - 1);
	}

	void GPU::clear() noexcept
	{
		if (_frame_buffer == nullptr)
		{
			set_error(gpu_debug::ErrorCode::InvalidOperation);
			return;
		}

		reset_render_stats();
		size_t pixelSize = _frame_buffer->_width * _frame_buffer->_height;
		std::fill_n(_frame_buffer->_color_buffer, pixelSize, RGBA(0, 0, 0, 0));
		std::fill_n(_frame_buffer->_depth_buffer, pixelSize, 1.0f);
	}

	uint32_t GPU::gen_buffer()
	{
		BufferObject* buffer = nullptr;
		try
		{
			buffer = new BufferObject();
		}
		catch (const std::bad_alloc&)
		{
			set_error(gpu_debug::ErrorCode::OutOfMemory);
			return 0;
		}

		++_buffer_counter;
		_buffer_map.insert(std::make_pair(_buffer_counter, buffer));

		return _buffer_counter;
	}

	void GPU::delete_buffer(uint32_t buffer_ID)
	{
		auto iter = _buffer_map.find(buffer_ID);

		if (iter != _buffer_map.end())
			delete iter->second;
		else
		{
			set_error(gpu_debug::ErrorCode::InvalidValue);
			return;
		}

		_buffer_map.erase(iter);
	}

	void GPU::bind_buffer(uint8_t buffer_type, uint32_t buffer_ID)
	{
		if (buffer_type == MAI_ARRAY_BUFFER)
		{
			if (buffer_ID != 0 && _buffer_map.find(buffer_ID) == _buffer_map.end())
			{
				set_error(gpu_debug::ErrorCode::InvalidValue);
				return;
			}
			_current_VBO = buffer_ID;
		}
		else if (buffer_type == MAI_ELEMENT_ARRAY_BUFFER)
		{
			if (buffer_ID != 0 && _buffer_map.find(buffer_ID) == _buffer_map.end())
			{
				set_error(gpu_debug::ErrorCode::InvalidValue);
				return;
			}
			_current_EBO = buffer_ID;
		}
		else
		{
			set_error(gpu_debug::ErrorCode::InvalidEnum);
		}
	}

	void GPU::buffer_data(uint8_t buffer_type, size_t data_size, void* data)
	{
		uint32_t buffer_ID = 0;

		if (buffer_type == MAI_ARRAY_BUFFER)
			buffer_ID = _current_VBO;
		else if (buffer_type == MAI_ELEMENT_ARRAY_BUFFER)
			buffer_ID = _current_EBO;
		else
		{
			set_error(gpu_debug::ErrorCode::InvalidEnum);
			return;
		}

		auto iter = _buffer_map.find(buffer_ID);
		if (iter == _buffer_map.end())
		{
			set_error(gpu_debug::ErrorCode::InvalidOperation);
			return;
		}

		BufferObject* buffer_object = iter->second;
		buffer_object->set_buffer_data(data_size, data);
	}

	uint32_t GPU::gen_vertex_array()
	{
		VertexArrayObject* vao = nullptr;
		try
		{
			vao = new VertexArrayObject();
		}
		catch (const std::bad_alloc&)
		{
			set_error(gpu_debug::ErrorCode::OutOfMemory);
			return 0;
		}

		++_VAO_counter;
		_VAO_map.insert(std::make_pair(_VAO_counter, vao));

		return _VAO_counter;
	}

	void GPU::delete_vertex_array(uint32_t VAO_ID)
	{
		auto iter = _VAO_map.find(VAO_ID);

		if (iter != _VAO_map.end())
			delete iter->second;
		else
		{
			set_error(gpu_debug::ErrorCode::InvalidValue);
			return;
		}

		_VAO_map.erase(iter);
	}

	void GPU::bind_vertex_array(uint32_t VAO_ID)
	{
		if (VAO_ID != 0 && _VAO_map.find(VAO_ID) == _VAO_map.end())
		{
			set_error(gpu_debug::ErrorCode::InvalidValue);
			return;
		}

		_current_VAO = VAO_ID;
	}

	void GPU::vertex_attribute_pointer(
		uint32_t binding, size_t item_size,
		size_t stride, size_t offset)
	{
		auto iter = _VAO_map.find(_current_VAO);
		if (iter == _VAO_map.end())
		{
			set_error(gpu_debug::ErrorCode::InvalidOperation);
			return;
		}

		auto VBO_iter = _buffer_map.find(_current_VBO);
		if (VBO_iter == _buffer_map.end())
		{
			set_error(gpu_debug::ErrorCode::InvalidOperation);
			return;
		}

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
		if (_frame_buffer == nullptr)
		{
			set_error(gpu_debug::ErrorCode::InvalidOperation);
			return;
		}

		if (_current_VAO == 0 || _shader == nullptr)
		{
			set_error(gpu_debug::ErrorCode::InvalidOperation);
			return;
		}

		if (draw_mode != MAI_DRAW_LINES && draw_mode != MAI_DRAW_TRIANGLES)
		{
			set_error(gpu_debug::ErrorCode::InvalidEnum);
			return;
		}

		if (count == 0)
			return;

		auto VAO_iter = _VAO_map.find(_current_VAO);

		if (VAO_iter == _VAO_map.end())
		{
			set_error(gpu_debug::ErrorCode::InvalidOperation);
			return;
		}

		const VertexArrayObject* vao = VAO_iter->second;

		auto EBO_iter = _buffer_map.find(_current_EBO);

		if (EBO_iter == _buffer_map.end())
		{
			set_error(gpu_debug::ErrorCode::InvalidOperation);
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
			set_error(gpu_debug::ErrorCode::InvalidEnum);
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
			set_error(gpu_debug::ErrorCode::InvalidEnum);
			break;
		}
	}

	void GPU::front_face(uint8_t value)
	{
		if (value != MAI_FRONT_FACE_CW && value != MAI_FRONT_FACE_CCW)
		{
			set_error(gpu_debug::ErrorCode::InvalidEnum);
			return;
		}

		_render_state._front_face = static_cast<uint8_t>(value);
	}

	void GPU::cull_face(uint8_t value)
	{
		if (value != MAI_FRONT_FACE && value != MAI_BACK_FACE)
		{
			set_error(gpu_debug::ErrorCode::InvalidEnum);
			return;
		}

		_render_state._cull_face = static_cast<uint8_t>(value);
	}

	void GPU::depth_function(uint8_t value)
	{
		if (value != MAI_DEPTH_LESS && value != MAI_DEPTH_GREATER)
		{
			set_error(gpu_debug::ErrorCode::InvalidEnum);
			return;
		}

		_render_state._depth_function = static_cast<uint8_t>(value);
	}

	uint32_t GPU::gen_texture()
	{
		Texture* texture = nullptr;
		try
		{
			texture = new Texture();
		}
		catch (const std::bad_alloc&)
		{
			set_error(gpu_debug::ErrorCode::OutOfMemory);
			return 0;
		}

		_texture_counter++;
		_texture_map.insert(std::make_pair(_texture_counter, texture));

		return _texture_counter;
	}

	void GPU::delete_texture(uint32_t tex_ID)
	{
		auto iter = _texture_map.find(tex_ID);
		if (iter != _texture_map.end())
			delete iter->second;
		else
		{
			set_error(gpu_debug::ErrorCode::InvalidValue);
			return;
		}

		_texture_map.erase(iter);
	}

	void GPU::bind_texture(uint32_t tex_ID)
	{
		if (tex_ID != 0 && _texture_map.find(tex_ID) == _texture_map.end())
		{
			set_error(gpu_debug::ErrorCode::InvalidValue);
			return;
		}

		_current_texture = tex_ID;
	}

	void GPU::tex_image_2D(uint32_t width, uint32_t height, void* data)
	{
		if (!_current_texture)
		{
			set_error(gpu_debug::ErrorCode::InvalidOperation);
			return;
		}

		if (width == 0 || height == 0 || data == nullptr)
		{
			set_error(gpu_debug::ErrorCode::InvalidValue);
			return;
		}

		auto iter = _texture_map.find(_current_texture);
		if (iter == _texture_map.end()) {
			set_error(gpu_debug::ErrorCode::InvalidOperation);
			return;
		}
		auto texture = iter->second;
		texture->set_buffer_data(width, height, data);
	}

	void GPU::tex_parameter(uint8_t param, uint32_t value)
	{
		if (!_current_texture)
		{
			set_error(gpu_debug::ErrorCode::InvalidOperation);
			return;
		}

		auto iter = _texture_map.find(_current_texture);
		if (iter == _texture_map.end())
		{
			set_error(gpu_debug::ErrorCode::InvalidOperation);
			return;
		}

		if (param != MAI_TEXTURE_FILTER && param != MAI_TEXTURE_WRAP_U && param != MAI_TEXTURE_WRAP_V)
		{
			set_error(gpu_debug::ErrorCode::InvalidEnum);
			return;
		}

		if (param == MAI_TEXTURE_FILTER &&
			value != MAI_TEXTURE_FILTER_NEAREST &&
			value != MAI_TEXTURE_FILTER_LINEAR)
		{
			set_error(gpu_debug::ErrorCode::InvalidEnum);
			return;
		}

		if ((param == MAI_TEXTURE_WRAP_U || param == MAI_TEXTURE_WRAP_V) &&
			value != MAI_TEXTURE_WRAP_REPEAT &&
			value != MAI_TEXTURE_WRAP_MIRROR)
		{
			set_error(gpu_debug::ErrorCode::InvalidEnum);
			return;
		}

		Texture* texture = iter->second;
		texture->set_parameter(param, value);
	}

	void GPU::draw_dimension(uint8_t value)
	{
		if (value != MAI_DRAW_2D && value != MAI_DRAW_3D)
		{
			set_error(gpu_debug::ErrorCode::InvalidEnum);
			return;
		}

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

}
