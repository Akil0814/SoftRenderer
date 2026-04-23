#include "fragment_stage.h"

namespace mai
{

	void FragmentStage::run(const DrawContext& context, const std::vector<VsOutput>& raster_outputs) const
	{
		FsOutput fs_output;
		size_t pixel_pos = 0;
		for (size_t i = 0; i < raster_outputs.size(); ++i)
		{
			context._shader.fragment_shader(raster_outputs[i], fs_output, context._texture_map);
			pixel_pos = static_cast<size_t>(fs_output._pixel_pos.y) * context._frame_buffer._width + static_cast<size_t>(fs_output._pixel_pos.x);

			if (context._state._enable_depth_test && !depth_test(context, fs_output))
				continue;

			RGBA color = fs_output._color;
			if (context._state._enable_blending)
				color = blend(context, fs_output);

			context._frame_buffer._color_buffer[pixel_pos] = color;
		}
	}

	bool FragmentStage::depth_test(const DrawContext& context, const FsOutput& output) const
	{
		size_t pixel_pos = static_cast<size_t>(output._pixel_pos.y) * context._frame_buffer._width + static_cast<size_t>(output._pixel_pos.x);
		float old_depth = context._frame_buffer._depth_buffer[pixel_pos];
		switch (context._state._depth_function)
		{
		case MAI_DEPTH_LESS:
			if (output._depth < old_depth)
			{
				context._frame_buffer._depth_buffer[pixel_pos] = output._depth;
				return true;
			}
			return false;
		case MAI_DEPTH_GREATER:
			if (output._depth > old_depth)
			{
				context._frame_buffer._depth_buffer[pixel_pos] = output._depth;
				return true;
			}
			return false;
		default:
			return false;
		}
	}

	RGBA FragmentStage::blend(const DrawContext& context, const FsOutput& output) const
	{
		RGBA result;

		size_t pixel_pos = static_cast<size_t>(output._pixel_pos.y) * context._frame_buffer._width + static_cast<size_t>(output._pixel_pos.x);
		RGBA dst = context._frame_buffer._color_buffer[pixel_pos];
		RGBA src = output._color;

		float weight = static_cast<float>(src._A) / 255.0f;

		result._R = static_cast<float>(src._R) * weight + static_cast<float>(dst._R) * (1.0f - weight);
		result._G = static_cast<float>(src._G) * weight + static_cast<float>(dst._G) * (1.0f - weight);
		result._B = static_cast<float>(src._B) * weight + static_cast<float>(dst._B) * (1.0f - weight);
		result._A = static_cast<float>(src._A) * weight + static_cast<float>(dst._A) * (1.0f - weight);

		return result;
	}

}
