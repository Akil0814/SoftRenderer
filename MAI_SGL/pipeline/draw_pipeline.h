#pragma once

#include "../base.h"
#include "draw_context.h"
#include "fragment_stage.h"
#include "primitive_stage.h"
#include "vertex_stage.h"

namespace mai
{

class DrawPipeline
{
public:
	void draw_elements(const DrawContext& context, const uint32_t& draw_mode,
		const uint32_t& first, const uint32_t& count) const;

private:
	VertexStage _vertex_stage;
	PrimitiveStage _primitive_stage;
	FragmentStage _fragment_stage;
};

}
