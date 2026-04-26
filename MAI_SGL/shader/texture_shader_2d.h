#pragma once

#include "shader.h"
#include "../math/math.h"

namespace mai
{

class TextureShader2D : public Shader
{
public:
	TextureShader2D();
	~TextureShader2D();

	VsOutput vertex_shader(
		const std::map<uint32_t, BindingDescription>& binding_map,
		const std::map<uint32_t, BufferObject*>& buffer_map,
		size_t index
	) override;

	void fragment_shader(
		const VsOutput& input,
		FsOutput& output,
		const std::map<uint32_t, Texture*>& textures,
		RenderStats& stats) override;

public:
	bool _modulate_vertex_color{ false };
	mai::mat4f _transform_matrix;
	uint32_t _diffuse_texture{ 0 };
};

}
