#pragma once

#include <vector>

#include "../base.h"
#include "../data_structures.h"
#include "draw_context.h"

namespace mai
{

// Planned extraction target for fragment shading, depth test, blending, and framebuffer writeback.
class FragmentStage
{
public:
	void run(const DrawContext& context, const std::vector<VsOutput>& raster_outputs) const;

private:
	bool depth_test(const DrawContext& context, const FsOutput& output) const;
	RGBA blend(const DrawContext& context, const FsOutput& output) const;
};

}
