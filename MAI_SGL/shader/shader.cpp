#include "shader.h"

namespace mai {

	vec4f Shader::get_vector(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		uint32_t attributeLocation,
		size_t index)
	{

		//鍙栧嚭鏈睘鎬х殑Description
		auto binding_iter = bindingMap.find(attributeLocation);

		if (binding_iter == bindingMap.end())
			assert(false);

		BindingDescription binding_description = binding_iter->second;

		//鍙栧嚭鏈睘鎬ф墍鍦ㄧ殑vbo
		uint32_t vbo_ID = binding_description._VBO_id;
		auto vbo_iter = bufferMap.find(vbo_ID);

		if (vbo_iter == bufferMap.end())
			assert(false);

		BufferObject* vbo = vbo_iter->second;

		//璁＄畻鏁版嵁鍦╲bo涓殑鍋忕Щ閲?
		size_t data_offset = binding_description._stride * index + binding_description._offset;
		size_t data_size = binding_description._item_size * sizeof(float);

		const byte* buffer = vbo->get_buffer() + data_offset;

		vec4f result;

		//鎷疯礉鍑洪渶瑕佺殑鏁版嵁锛屾渶澶?涓猣loat
		memcpy(&result, buffer, data_size);

		return result;
	}


}
