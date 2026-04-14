#include "shader.h"

namespace mai {

	vec4f Shader::get_vector(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		const uint32_t& attributeLocation,
		const uint32_t& index)
	{

		//取出本属性的Description
		auto binding_iter = bindingMap.find(attributeLocation);

		if (binding_iter == bindingMap.end())
			assert(false);

		BindingDescription binding_description = binding_iter->second;

		//取出本属性所在的vbo
		uint16_t vbo_ID = binding_description._VBO_id;
		auto vbo_iter = bufferMap.find(vbo_ID);

		if (vbo_iter == bufferMap.end())
			assert(false);

		BufferObject* vbo = vbo_iter->second;

		//计算数据在vbo中的偏移量
		uint32_t data_offset = binding_description._stride * index + binding_description._offset;
		uint32_t data_size = binding_description._item_size * sizeof(float);

		const byte* buffer = vbo->get_buffer() + data_offset;

		vec4f result;

		//拷贝出需要的数据，最多4个float
		memcpy(&result, buffer, data_size);

		return result;
	}


}
