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
		const std::map<uint32_t, BindingDescription>& binding_map,
		const std::map<uint32_t, BufferObject*>& buffer_map,
		size_t index
	) = 0;

	virtual void fragment_shader(const VsOutput& input, FsOutput& output,
		const std::map<uint32_t, Texture*>& textures) = 0;

public:

	vec4f get_vector(
		const std::map<uint32_t, BindingDescription>& binding_map,
		const std::map<uint32_t, BufferObject*>& buffer_map,
		uint32_t attribute_location,
		size_t index);

	RGBA vector_to_RGBA(const vec4f& v) {
		RGBA color;
		color._R = v.x * 255.0;
		color._G = v.y * 255.0;
		color._B = v.z * 255.0;
		color._A = v.w * 255.0;

		return color;
	}

	bool has_attribute(
		const std::map<uint32_t, BindingDescription>& binding_map,
		uint32_t attribute_location)
	{
		return binding_map.find(attribute_location) != binding_map.end();
	}
};

}
