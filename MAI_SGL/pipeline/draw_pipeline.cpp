#include "draw_pipeline.h"

#include "../core/gpu_profiler.h"

#include <vector>

namespace mai
{

	void DrawPipeline::draw_elements(
		const DrawContext& context, uint8_t draw_mode,
		size_t first, size_t count) const
	{
		std::vector<VsOutput> vs_outputs{};
		{
			ScopedTimer timer(context._stats._vertex_ms);
			_vertex_stage.run(context, first, count, vs_outputs);
		}
		if (vs_outputs.empty())
			return;

		std::vector<VsOutput> raster_outputs{};
		{
			ScopedTimer timer(context._stats._primitive_ms);
			_primitive_stage.run(context, draw_mode, vs_outputs, raster_outputs);
		}
		if (raster_outputs.empty())
			return;

		{
			ScopedTimer timer(context._stats._fragment_ms);
			_fragment_stage.run(context, raster_outputs);
		}
	}
}
