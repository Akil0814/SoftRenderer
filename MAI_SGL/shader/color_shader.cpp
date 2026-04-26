#include "color_shader.h"

namespace mai {

	ColorShader::ColorShader() {}
	ColorShader::~ColorShader() {}

	VsOutput ColorShader::vertex_shader(
		const std::map<uint32_t, BindingDescription>& binding_map,
		const std::map<uint32_t, BufferObject*>& buffer_map,
		size_t index
	)
	{
		VsOutput output;

		vec4f position = get_vector(binding_map, buffer_map, 0, index);

		position.w = 1.0f;

		vec4f color = get_vector(binding_map, buffer_map, 1, index);
		vec4f UV = get_vector(binding_map, buffer_map, 2, index);

		output._position = _projection_matrix * _view_matrix * _model_matrix * position;
		output._color = color;
		output._UV = UV;

		return output;
	}

	void ColorShader::fragment_shader(
		const VsOutput& input, FsOutput& output,
		const std::map<uint32_t, Texture*>& textures,
		RenderStats& stats)
	{
		(void)textures;
		(void)stats;
		output._pixel_pos.x = static_cast<int>(input._position.x);
		output._pixel_pos.y = static_cast<int>(input._position.y);
		output._depth = input._position.z;
		output._color = vector_to_RGBA(input._color);
	}

}
