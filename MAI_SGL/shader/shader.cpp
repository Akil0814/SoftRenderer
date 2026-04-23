#include "shader.h"

namespace mai {

	vec4f Shader::get_vector(
		const std::map<uint32_t, BindingDescription>& binding_map,
		const std::map<uint32_t, BufferObject*>& buffer_map,
		uint32_t attribute_location,
		size_t index)
	{

		auto binding_iter = binding_map.find(attribute_location);

		if (binding_iter == binding_map.end())
			assert(false);

		BindingDescription binding_description = binding_iter->second;

		uint32_t VBO_ID = binding_description._VBO_ID;
		auto VBO_iter = buffer_map.find(VBO_ID);

		if (VBO_iter == buffer_map.end())
			assert(false);

		BufferObject* VBO = VBO_iter->second;

		size_t data_offset = binding_description._stride * index + binding_description._offset;
		size_t data_size = binding_description._item_size * sizeof(float);

		const byte* buffer = VBO->get_buffer() + data_offset;

		vec4f result;

		memcpy(&result, buffer, data_size);

		return result;
	}


}
