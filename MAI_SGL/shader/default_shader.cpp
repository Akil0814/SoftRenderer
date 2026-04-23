#include "default_shader.h"

namespace mai {

	DefaultShader::DefaultShader() {}
	DefaultShader::~DefaultShader() {}

	VsOutput DefaultShader::vertex_shader(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		size_t index
	)
	{
		VsOutput output;

		//鍙栧嚭Attribute鏁板€?
		vec4f position = get_vector(bindingMap, bufferMap, 0, index);

		//鍙樺寲涓洪綈娆″潗鏍?
		position.w = 1.0f;

		vec4f color = get_vector(bindingMap, bufferMap, 1, index);
		vec4f uv = get_vector(bindingMap, bufferMap, 2, index);

		output._position = _projection_matrix * _view_matrix * _model_matrix * position;
		output._color = color;
		output._UV = uv;

		return output;
	}

	void DefaultShader::fragment_shader(
		const VsOutput& input, FsOutput& output,
		const std::map<uint32_t, Texture*>& textures)
	{
		(void)textures;
		output._pixel_pos.x = static_cast<int>(input._position.x);
		output._pixel_pos.y = static_cast<int>(input._position.y);
		output._depth = input._position.z;
		output._color = vector_to_RGBA(input._color);
	}

}
