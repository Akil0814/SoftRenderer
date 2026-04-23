#pragma once
#include<map>

#include "../base.h"
#include "../data_structures.h"
#include "../resource/buffer_object.h"
#include "../resource/texture.h"

namespace mai
{


class Shader
{
public:
	Shader() {}
	~Shader() {}
	virtual VsOutput vertex_shader(
		//VAO当中的bindingMap
		const std::map<uint32_t, BindingDescription>& bindingMap,
		//VBO当中的bindingMap
		const std::map<uint32_t, BufferObject*>& bufferMap,
		//当前要处理的顶点的index
		const uint32_t& index
	) = 0;

	virtual void fragment_shader(const VsOutput& input, FsOutput& output,
		const std::map<uint32_t,Texture*>& textures) = 0;

public:

	vec4f get_vector(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		const uint32_t& attributeLocation,//当前属性的编号
		const uint32_t& index);//当前顶点编号

	RGBA vector_to_RGBA(const vec4f& v) {
		RGBA color;
		color._R = v.x * 255.0;
		color._G = v.y * 255.0;
		color._B = v.z * 255.0;
		color._A = v.w * 255.0;

		return color;
	}
};

}

