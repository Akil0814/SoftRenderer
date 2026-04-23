#include "draw_pipeline.h"

#include <vector>

namespace mai
{

	void DrawPipeline::draw_elements(
		const DrawContext& context, const uint32_t& draw_mode,
		const uint32_t& first, const uint32_t& count) const
	{
		std::vector<VsOutput> vs_outputs{};
		_vertex_stage.run(context, first, count, vs_outputs);
		if (vs_outputs.empty())
			return;

		std::vector<VsOutput> raster_outputs{};
		_primitive_stage.run(context, draw_mode, vs_outputs, raster_outputs);
		if (raster_outputs.empty())
			return;

		_fragment_stage.run(context, raster_outputs);
	}

}
