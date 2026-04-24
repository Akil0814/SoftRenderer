#include "texture_shader_2D.h"

namespace mai
{

	TextureShader2D::TextureShader2D() {}
	TextureShader2D::~TextureShader2D() {}

	VsOutput TextureShader2D::vertex_shader(
		const std::map<uint32_t, BindingDescription>& binding_map,
		const std::map<uint32_t, BufferObject*>& buffer_map,
		size_t index)
	{
	VsOutput output;

	vec4f position = get_vector(binding_map, buffer_map, 0, index);
	position.z = 0.0f;
	position.w = 1.0f;
	mai::vec4f color = get_vector(binding_map, buffer_map, 1, index);
	mai::vec2f UV(get_vector(binding_map, buffer_map, 2, index));

	output._position = _transform_matrix * position;
	output._color = color;
	output._UV = UV;

	return output;
	}

	void TextureShader2D::fragment_shader(
		const VsOutput& input,
		FsOutput& output,
		const std::map<uint32_t, Texture*>& textures)
	{
		output._pixel_pos.x = static_cast<int>(input._position.x);
		output._pixel_pos.y = static_cast<int>(input._position.y);
		output._depth = input._position.z;

		auto iter = textures.find(_diffuse_texture);
		if (iter == textures.end() || iter->second == nullptr)
		{
			output._color = vector_to_RGBA(input._color);
			return;
		}

		Texture* texture = iter->second;

		mai::vec4f diffuseColor = texture->get_color(input._UV.x, input._UV.y);
		output._color = vector_to_RGBA(diffuseColor);
	}

}
