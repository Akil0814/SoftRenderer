#pragma once
#include "shader.h"
#include "../math/math.h"

namespace mai
{

class TextureShader :public Shader
{
public:
	TextureShader();
	~TextureShader();

	VsOutput vertex_shader(
		const std::map<uint32_t, BindingDescription>& binding_map,
		const std::map<uint32_t, BufferObject*>& buffer_map,
		size_t index
	)override;

	void fragment_shader(
		const VsOutput& input,
		FsOutput& output,
		const std::map<uint32_t, Texture*>& textures,
		RenderStats& stats) override;

public:
	mai::mat4f _model_matrix;
	mai::mat4f _view_matrix;
	mai::mat4f _projection_matrix;

	uint32_t _diffuse_texture{ 0 };
};

}
