#pragma once

#include <vector>

#include "../base.h"
#include "../data_structures.h"
#include "draw_context.h"

namespace mai
{

// Planned extraction target for GPU::vertex_shader_stage and related input assembly.
class VertexStage
{
public:
	void run(const DrawContext& context, size_t first, size_t count,
		std::vector<VsOutput>& vs_outputs) const;
};

}
