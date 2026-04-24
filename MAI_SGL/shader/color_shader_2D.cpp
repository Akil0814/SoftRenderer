#include "color_shader_2D.h"

namespace mai
{

ColorShader2D::ColorShader2D() {}
ColorShader2D::~ColorShader2D() {}

VsOutput ColorShader2D::vertex_shader(
	const std::map<uint32_t, BindingDescription>& binding_map,
	const std::map<uint32_t, BufferObject*>& buffer_map,
	size_t index)
{
	VsOutput output;

	vec4f position = get_vector(binding_map, buffer_map, 0, index);
	position.z = 0.0f;
	position.w = 1.0f;

	vec4f color{ 1.0f, 1.0f, 1.0f, 1.0f };
	if (has_attribute(binding_map, 1))
	{
		color = get_vector(binding_map, buffer_map, 1, index);
	}

	vec2f UV{ 0.0f, 0.0f };
	if (has_attribute(binding_map, 2))
	{
		UV = get_vector(binding_map, buffer_map, 2, index);
	}

	output._position = _transform_matrix * position;
	output._color = color;
	output._UV = UV;

	return output;
}

void ColorShader2D::fragment_shader(
	const VsOutput& input,
	FsOutput& output,
	const std::map<uint32_t, Texture*>& textures)
{
	(void)textures;
	output._pixel_pos.x = static_cast<int>(input._position.x);
	output._pixel_pos.y = static_cast<int>(input._position.y);
	output._depth = input._position.z;
	output._color = vector_to_RGBA(input._color);
}

}
