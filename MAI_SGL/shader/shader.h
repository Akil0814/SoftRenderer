#pragma once
#include <algorithm>
#include<map>

#include "../base.h"
#include "../core/gpu_stats.h"
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
		const std::map<uint32_t, Texture*>& textures,
		RenderStats& stats) = 0;

public:

	vec4f get_vector(
		const std::map<uint32_t, BindingDescription>& binding_map,
		const std::map<uint32_t, BufferObject*>& buffer_map,
		uint32_t attribute_location,
		size_t index);

	RGBA vector_to_RGBA(const vec4f& v) {
		RGBA color;
		color._R = static_cast<byte>(std::clamp(v.x, 0.0f, 1.0f) * 255.0f);
		color._G = static_cast<byte>(std::clamp(v.y, 0.0f, 1.0f) * 255.0f);
		color._B = static_cast<byte>(std::clamp(v.z, 0.0f, 1.0f) * 255.0f);
		color._A = static_cast<byte>(std::clamp(v.w, 0.0f, 1.0f) * 255.0f);

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
