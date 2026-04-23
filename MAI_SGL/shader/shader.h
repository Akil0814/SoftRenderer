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
		//VAO褰撲腑鐨刡indingMap
		const std::map<uint32_t, BindingDescription>& bindingMap,
		//VBO褰撲腑鐨刡indingMap
		const std::map<uint32_t, BufferObject*>& bufferMap,
		//褰撳墠瑕佸鐞嗙殑椤剁偣鐨刬ndex
		size_t index
	) = 0;

	virtual void fragment_shader(const VsOutput& input, FsOutput& output,
		const std::map<uint32_t,Texture*>& textures) = 0;

public:

	vec4f get_vector(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		uint32_t attributeLocation,//褰撳墠灞炴€х殑缂栧彿
		size_t index);//褰撳墠椤剁偣缂栧彿

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
