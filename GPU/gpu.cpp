#include "gpu.h"
#include "raster.h"
#include "clipper.h"

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

	void GPU::init_surface(const uint32_t& width, const uint32_t& height, void* buffer)
	{
		_frame_buffer = new FrameBuffer(width, height, buffer);
		_screen_matrix = screen_matrix<float>(width - 1, height - 1);
	}

	void GPU::clear() noexcept
	{
		size_t pixelSize = _frame_buffer->_width * _frame_buffer->_height;
		std::fill_n(_frame_buffer->_color_buffer, pixelSize, RGBA(0, 0, 0, 0));
		std::fill_n(_frame_buffer->_depth_buffer, pixelSize, 1.0f);
	}

	void GPU::printVAO(const uint32_t& vaoID)
	{
		auto iter = _VAO_map.find(vaoID);
		if (iter != _VAO_map.end())
			iter->second->print();
	}

	uint32_t GPU::gen_buffer()
	{
		++_buffer_counter;
		_buffer_map.insert(std::make_pair(_buffer_counter, new BufferObject()));

		return _buffer_counter;
	}

	void GPU::delete_buffer(const uint32_t& buffer_ID)
	{
		auto iter = _buffer_map.find(buffer_ID);

		if (iter != _buffer_map.end())
			delete iter->second;
		else
			return;

		_buffer_map.erase(iter);
	}

	void GPU::bind_buffer(const uint32_t& buffer_type, const uint32_t& bufferID)
	{
		if (buffer_type == MAI_ARRAY_BUFFER)
			_current_VBO = bufferID;

		else if (buffer_type == MAI_ELEMENT_ARRAY_BUFFER)
			_current_EBO = bufferID;
	}

	void GPU::buffer_data(const uint32_t& buffer_type, size_t data_size, void* data)
	{
		uint32_t bufferID = 0;

		if (buffer_type == MAI_ARRAY_BUFFER)
			bufferID = _current_VBO;
		else if (buffer_type == MAI_ELEMENT_ARRAY_BUFFER)
			bufferID = _current_EBO;
		else
			assert(false);

		auto iter = _buffer_map.find(bufferID);
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

	void GPU::delete_vertex_array(const uint32_t& VAO_ID)
	{
		auto iter = _VAO_map.find(VAO_ID);

		if (iter != _VAO_map.end())
			delete iter->second;
		else
			return;

		_VAO_map.erase(iter);
	}

	void GPU::bind_vertex_array(const uint32_t& VAO_ID)
	{
		_current_VAO = VAO_ID;
	}

	void GPU::vertex_attribute_pointer(
		const uint32_t& binding, const uint32_t& item_size,
		const uint32_t& stride, const uint32_t& offset)
	{
		auto iter = _VAO_map.find(_current_VAO);
		if (iter == _VAO_map.end())
			assert(false);

		auto vboIter = _buffer_map.find(_current_VBO);
		if (vboIter == _buffer_map.end())
			assert(false);

		auto vao = iter->second;
		vao->set(binding, _current_VBO, item_size, stride, offset);
	}

	void GPU::use_program(Shader* shader)
	{
		_shader = shader;
	}

	void GPU::draw_element(const uint32_t& draw_mode, const uint32_t& first, const uint32_t& count)
	{
		if (_current_VAO == 0 || _shader == nullptr || count == 0)
			return;

		//1 get vao
		auto vaoIter = _VAO_map.find(_current_VAO);

		if (vaoIter == _VAO_map.end())
		{
			std::cerr << "Error: current vao is invalid!" << std::endl;
			return;
		}

		const VertexArrayObject* vao = vaoIter->second;
		auto bindingMap = vao->get_binding_map();

		//2 get ebo
		auto eboIter = _buffer_map.find(_current_EBO);

		if (eboIter == _buffer_map.end())
		{
			std::cerr << "Error: current ebo is invalid!" << std::endl;
			return;
		}

		const BufferObject* ebo = eboIter->second;

		/*
		* VertexShader处理阶段
		* 作用：
		*	按照输入的Ebo的index顺序来处理顶点，依次通过vsShader，
			得到的输出结果按序放入vsOutputs中
		*/
		std::vector<VsOutput> vsOutputs{};
		vertex_shader_stage(vao, ebo, first, count, vsOutputs);

		if (vsOutputs.empty())
			return;

		/*
		* Clip Space处理阶段
		* 作用：
		*	在剪裁空间，对所有输出的图元进行剪裁拼接等
		*/
		std::vector<VsOutput> clipOutputs{};
		Clipper::do_clip_space(draw_mode, vsOutputs, clipOutputs);
		if (clipOutputs.empty())
			return;

		vsOutputs.clear();

		/*
		* NDC处理阶段
		* 作用：
		*	将顶点转化到NDC下
		*/
		for (auto& output : clipOutputs)
		{
			perspective_division(output);
		}

		std::vector<VsOutput> cull_outputs = clipOutputs;
		if (draw_mode == MAI_DRAW_TRIANGLES && _enable_cull_face)
		{
			cull_outputs.clear();
			for (uint32_t i = 0; i < clipOutputs.size() - 2; i += 3)
			{
				if (Clipper::cull_face(_front_face, _cull_face, clipOutputs[i], clipOutputs[i + 1], clipOutputs[i + 2]))
				{
					auto start = clipOutputs.begin() + i;
					auto end = clipOutputs.begin() + i + 3;
					cull_outputs.insert(cull_outputs.end(), start, end);
				}
			}
		}

		/*
		* 屏幕映射处理阶段
		* 作用：
		*	将NDC下的点，通过screenMatrix，转化到屏幕空间
		*/
		for (auto& output : cull_outputs)
		{
			screen_mapping(output);
		}

		/*
		* 光栅化处理阶段
		* 作用：
		*	离散出所有需要的Fragment
		*/
		std::vector<VsOutput> rasterOutputs;
		Raster::rasterize(draw_mode, cull_outputs, rasterOutputs);


		if (rasterOutputs.empty())
			return;
		/*
		* 透视恢复处理阶段
		* 作用：
		*	离散出来的像素插值结果，需要乘以自身的w值恢复到正常态
		*/
		for (auto& output : rasterOutputs)
		{
			perspective_recover(output);
		}


		/*
		* 颜色输出处理阶段
		* 作用：
		*	 将颜色进行输出
		*/
		FsOutput fsOutput;
		uint32_t pixelPos = 0;
		for (uint32_t i = 0; i < rasterOutputs.size(); ++i) {
			_shader->fragment_shader(rasterOutputs[i], fsOutput, _texture_map);
			pixelPos = fsOutput._pixel_pos.y * _frame_buffer->_width + fsOutput._pixel_pos.x;
			
			if (_enable_depth_test && !depth_test(fsOutput))
				continue;

			RGBA color = fsOutput._color;
			if (_enable_blending)
				color = blend(fsOutput);
			
			_frame_buffer->_color_buffer[pixelPos] = color;
		}
	}

	void GPU::vertex_shader_stage(
		const VertexArrayObject* vao, const BufferObject* ebo,
		const uint32_t first, const uint32_t count,
		std::vector<VsOutput>& vsOutputs)
	{
		auto bindingMap = vao->get_binding_map();
		byte* indicesData = ebo->get_buffer();

		uint32_t index = 0;
		for (uint32_t i = first; i < first + count; ++i) {
			//获取Ebo中第i个index
			size_t indicesOffset = i * sizeof(uint32_t);
			memcpy(&index, indicesData + indicesOffset, sizeof(uint32_t));

			VsOutput output = _shader->vertex_shader(bindingMap, _buffer_map, index);
			vsOutputs.push_back(output);
		}
	}

	void GPU::perspective_division(VsOutput& vsOutput)
	{
		vsOutput._inv_w = 1.0f / vsOutput._position.w;

		vsOutput._position *= vsOutput._inv_w;
		vsOutput._position.w = 1.0f;

		vsOutput._color *= vsOutput._inv_w;
		vsOutput._UV *= vsOutput._inv_w;

		trim(vsOutput);
	}


	void GPU::perspective_recover(VsOutput& vsOutput)
	{
		vsOutput._color /= vsOutput._inv_w;
		vsOutput._UV /= vsOutput._inv_w;
	}

	void GPU::screen_mapping(VsOutput& vsOutput)
	{
		vsOutput._position = _screen_matrix * vsOutput._position;
	}

	void GPU::trim(VsOutput& vsOutput)
	{
		////修剪毛刺
		if (vsOutput._position.x < -1.0f)
			vsOutput._position.x = -1.0f;

		if (vsOutput._position.x > 1.0f)
			vsOutput._position.x = 1.0f;

		if (vsOutput._position.y < -1.0f)
			vsOutput._position.y = -1.0f;

		if (vsOutput._position.y > 1.0f)
			vsOutput._position.y = 1.0f;

		if (vsOutput._position.z < -1.0f)
			vsOutput._position.z = -1.0f;

		if (vsOutput._position.z > 1.0f)
			vsOutput._position.z = 1.0f;
	}

	bool GPU::depth_test(const FsOutput& output)
	{
		uint32_t pixelPos = output._pixel_pos.y * _frame_buffer->_width + output._pixel_pos.x;
		float oldDepth = _frame_buffer->_depth_buffer[pixelPos];
		switch (_depth_function)
		{
		case MAI_DEPTH_LESS:
			if (output._depth < oldDepth) {
				_frame_buffer->_depth_buffer[pixelPos] = output._depth;
				return true;
			}
			else {
				return false;
			}
			break;
		case MAI_DEPTH_GREATER:
			if (output._depth > oldDepth) {
				_frame_buffer->_depth_buffer[pixelPos] = output._depth;
				return true;
			}
			else {
				return false;
			}
			break;
		default:
			return false;
			break;
		}
	}

	void GPU::enable(const uint32_t& value)
	{
		switch (value)
		{
		case MAI_CULL_FACE:
			_enable_cull_face = true;
			break;
		case MAI_DEPTH_TEST:
			_enable_depth_test = true;
			break;
		case MAI_BLENDING:
			_enable_blending = true;
			break;
		default:
			break;
		}
	}

	void GPU::disable(const uint32_t& value)
	{
		switch (value)
		{
		case MAI_CULL_FACE:
			_enable_cull_face = false;
			break;
		case MAI_DEPTH_TEST:
			_enable_depth_test = false;
			break;
		case MAI_BLENDING:
			_enable_blending = false;
			break;
		default:
			break;
		}
	}

	RGBA GPU::blend(const FsOutput& output)
	{
		RGBA result;

		uint32_t pixelPos = output._pixel_pos.y * _frame_buffer->_width + output._pixel_pos.x;
		RGBA dst = _frame_buffer->_color_buffer[pixelPos];
		RGBA src = output._color;

		float weight = static_cast<float>(src._A) / 255.0f;

		result._R = static_cast<float>(src._R) * weight + static_cast<float>(dst._R) * (1.0f - weight);
		result._G = static_cast<float>(src._G) * weight + static_cast<float>(dst._G) * (1.0f - weight);
		result._B = static_cast<float>(src._B) * weight + static_cast<float>(dst._B) * (1.0f - weight);
		result._A = static_cast<float>(src._A) * weight + static_cast<float>(dst._A) * (1.0f - weight);

		return result;
	}

	void GPU::front_face(const uint32_t& value)
	{
		_front_face = value;
	}

	void GPU::cull_face(const uint32_t& value)
	{
		_cull_face = value;
	}

	void GPU::depth_function(const uint32_t& value)
	{
		_depth_function = value;
	}

	uint32_t GPU::get_texture()
	{
		_texture_counter++;
		_texture_map.insert(std::make_pair(_texture_counter, new Texture()));

		return _texture_counter;
	}

	void GPU::delete_texture(const uint32_t& tex_id)
	{
		auto iter = _texture_map.find(tex_id);
		if (iter != _texture_map.end())
			delete iter->second;
		else
			return;

		_texture_map.erase(iter);
	}

	void GPU::bind_texture(const uint32_t& tex_id)
	{
		_current_texture = tex_id;
	}

	void GPU::tex_image_2D(const uint32_t& width, const uint32_t& height, void* data)
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

	void GPU::tex_parameter(const uint32_t& param, const uint32_t& value)
	{
		if (!_current_texture) 
			return;

		auto iter = _texture_map.find(_current_texture);
		if (iter == _texture_map.end())
			return;

		Texture* texture = iter->second;
		texture->set_parameter(param, value);
	}

	void GPU::draw_mod(const uint32_t& value)
	{
		_draw_mod = value;
	}
}
