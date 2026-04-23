#include "texture_shader_2d.h"

namespace mai
{

	TextureShader2D::TextureShader2D() {}
	TextureShader2D::~TextureShader2D() {}

	VsOutput TextureShader2D::vertex_shader(
		const std::map<uint32_t, BindingDescription>& binding_map,
		const std::map<uint32_t, BufferObject*>& buffer_map,
		size_t index)
	{
		(void)binding_map;
		(void)buffer_map;
		(void)index;

		// TODO: Implement 2D vertex transformation and attribute extraction.
		return VsOutput{};
	}

	void TextureShader2D::fragment_shader(
		const VsOutput& input,
		FsOutput& output,
		const std::map<uint32_t, Texture*>& textures)
	{
		(void)input;
		(void)textures;

		// TODO: Implement 2D texture sampling and pixel output.
		output = FsOutput{};
	}

}
