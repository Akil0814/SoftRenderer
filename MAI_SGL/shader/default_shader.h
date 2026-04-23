#pragma once
#include "shader.h"
#include "../math/math.h"

namespace mai{

class DefaultShader :public Shader
{
public:
	DefaultShader();
	~DefaultShader();

	VsOutput vertex_shader(
		const std::map<uint32_t, BindingDescription>& binding_map,
		const std::map<uint32_t, BufferObject*>& buffer_map,
		size_t index
	)override;

	void fragment_shader(
		const VsOutput& input, FsOutput& output,
		const std::map<uint32_t, Texture*>& textures)override;

public:
	//uniforms
	mat4f _model_matrix;
	mat4f _view_matrix;
	mat4f _projection_matrix;
};

}
