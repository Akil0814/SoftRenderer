#include "vertex_stage.h"

#include <cstring>

namespace mai
{

	void VertexStage::run(
		const DrawContext& context, size_t first, size_t count,
		std::vector<VsOutput>& vs_outputs) const
	{
		auto binding_map = context._vao.get_binding_map();
		byte* indices_data = context._ebo.get_buffer();

		uint32_t index = 0;
		for (size_t i = first; i < first + count; ++i)
		{
			size_t indices_offset = i * sizeof(uint32_t);
			memcpy(&index, indices_data + indices_offset, sizeof(uint32_t));

			VsOutput output = context._shader.vertex_shader(binding_map, context._buffer_map, index);
			vs_outputs.push_back(output);
		}
	}

}
