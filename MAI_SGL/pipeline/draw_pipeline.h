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
	void draw_elements(const DrawContext& context, uint8_t draw_mode,
		size_t first, size_t count) const;

private:
	VertexStage _vertex_stage;
	PrimitiveStage _primitive_stage;
	FragmentStage _fragment_stage;
};

}
