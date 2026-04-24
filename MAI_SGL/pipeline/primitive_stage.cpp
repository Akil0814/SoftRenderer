#include "primitive_stage.h"

#include "clipper.h"
#include "raster.h"

namespace mai
{

	void PrimitiveStage::run(
		const DrawContext& context, uint8_t draw_mode,
		const std::vector<VsOutput>& vs_outputs, std::vector<VsOutput>& raster_outputs) const
	{
		if (context._state._draw_dimension == MAI_DRAW_3D)
			run_3D(context, draw_mode, vs_outputs, raster_outputs);
		else if (context._state._draw_dimension == MAI_DRAW_2D)
			run_2D(context, draw_mode, vs_outputs, raster_outputs);
		else
			return;
	}

	void  PrimitiveStage::run_3D(const DrawContext& context, uint8_t draw_mode,
		const std::vector<VsOutput>& vs_outputs, std::vector<VsOutput>& raster_outputs) const
	{
		std::vector<VsOutput> clip_outputs{};
		Clipper::do_clip_space(draw_mode, vs_outputs, clip_outputs);
		if (clip_outputs.empty())
			return;

		for (auto& output : clip_outputs)
		{
			perspective_division(output);
		}

		std::vector<VsOutput> cull_outputs = clip_outputs;
		if (draw_mode == MAI_DRAW_TRIANGLES && context._state._enable_cull_face)
		{
			cull_outputs.clear();
			for (uint32_t i = 0; i < clip_outputs.size() - 2; i += 3)
			{
				if (Clipper::cull_face(
					context._state._front_face,
					context._state._cull_face,
					clip_outputs[i],
					clip_outputs[i + 1],
					clip_outputs[i + 2]))
				{
					auto start = clip_outputs.begin() + i;
					auto end = clip_outputs.begin() + i + 3;
					cull_outputs.insert(cull_outputs.end(), start, end);
				}
			}
		}

		for (auto& output : cull_outputs)
		{
			screen_mapping(context, output);
		}

		Raster::rasterize(context,draw_mode, cull_outputs, raster_outputs);
		if (raster_outputs.empty())
			return;

		for (auto& output : raster_outputs)
		{
			perspective_recover(output);
		}
	}

	void  PrimitiveStage::run_2D(const DrawContext& context, uint8_t draw_mode,
		const std::vector<VsOutput>& vs_outputs, std::vector<VsOutput>& raster_outputs) const
	{
		std::vector<VsOutput> clip_outputs{};
		Clipper::do_clip_space(draw_mode, vs_outputs, clip_outputs);
		if (clip_outputs.empty())
			return;

		for (auto& output : clip_outputs)
		{
			screen_mapping(context, output);
		}

		Raster::rasterize(context,draw_mode, clip_outputs, raster_outputs);

		if (raster_outputs.empty())
			return;

	}



	void PrimitiveStage::perspective_division(VsOutput& vs_output) const
	{
		vs_output._inv_w = 1.0f / vs_output._position.w;

		vs_output._position *= vs_output._inv_w;
		vs_output._position.w = 1.0f;

		vs_output._color *= vs_output._inv_w;
		vs_output._UV *= vs_output._inv_w;

		trim(vs_output);
	}

	void PrimitiveStage::perspective_recover(VsOutput& vs_output) const
	{
		vs_output._color /= vs_output._inv_w;
		vs_output._UV /= vs_output._inv_w;
	}

	void PrimitiveStage::screen_mapping(const DrawContext& context, VsOutput& vs_output) const
	{
		vs_output._position = context._screen_matrix * vs_output._position;
	}

	void PrimitiveStage::trim(VsOutput& vs_output) const
	{
		if (vs_output._position.x < -1.0f)
			vs_output._position.x = -1.0f;

		if (vs_output._position.x > 1.0f)
			vs_output._position.x = 1.0f;

		if (vs_output._position.y < -1.0f)
			vs_output._position.y = -1.0f;

		if (vs_output._position.y > 1.0f)
			vs_output._position.y = 1.0f;

		if (vs_output._position.z < -1.0f)
			vs_output._position.z = -1.0f;

		if (vs_output._position.z > 1.0f)
			vs_output._position.z = 1.0f;
	}

}
