#include "texture_shader.h"

namespace mai
{

	TextureShader::TextureShader() {}
	TextureShader::~TextureShader() {}

	VsOutput TextureShader::vertex_shader(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		const uint32_t& index
	)
	{
		VsOutput output;

		//取出Attribute数值
		mai::vec4f position = get_vector(bindingMap, bufferMap, 0, index);

		//变化为齐次坐标 
		position.w = 1.0f;
		mai::vec4f color = get_vector(bindingMap, bufferMap, 1, index);

		mai::vec2f uv(get_vector(bindingMap, bufferMap, 2, index));

		output._position = _projection_matrix * _view_matrix * _model_matrix * position;
		output._color = color;
		output._UV = uv;

		return output;
	}

	void TextureShader::fragment_shader(const VsOutput& input, FsOutput& output, const std::map<uint32_t, Texture*>& textures)
	{
		output._pixel_pos.x = static_cast<int>(input._position.x);
		output._pixel_pos.y = static_cast<int>(input._position.y);
		output._depth = input._position.z;

		//取出texture
		auto iter = textures.find(_diffuse_texture);
		if (iter == textures.end() || iter->second == nullptr)
		{
			output._color = vector_to_RGBA(input._color);
			return;
		}

		Texture* texture = iter->second;

		//计算颜色
		mai::vec4f diffuseColor = texture->get_color(input._UV.x, input._UV.y);
		output._color = vector_to_RGBA(diffuseColor);
	}

}
