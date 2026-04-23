#include "draw_pipeline.h"

#include <vector>

namespace mai
{

	void DrawPipeline::draw_elements(
		const DrawContext& context, uint32_t draw_mode,
		size_t first, size_t count) const
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
