#pragma once

#include <vector>

#include "../base.h"
#include "../data_structures.h"
#include "draw_context.h"

namespace mai
{

// Planned extraction target for clip / perspective divide / cull / screen mapping / rasterize.
class PrimitiveStage
{
public:
	void run(const DrawContext& context, const uint32_t& draw_mode,
		const std::vector<VsOutput>& vs_outputs, std::vector<VsOutput>& raster_outputs) const;

private:
	void perspective_division(VsOutput& vs_output) const;
	void perspective_recover(VsOutput& vs_output) const;
	void screen_mapping(const DrawContext& context, VsOutput& vs_output) const;
	void trim(VsOutput& vs_output) const;
};

}
