#include "gpu.h"

namespace mai
{

RenderStats GPU::get_render_stats() const
{
	return _render_stats;
}

RenderStats GPU::get_summary_stats() const
{
	return _summary_render_stats;
}

void GPU::add_rasterized_pixels(uint64_t count) noexcept
{
	_render_stats._frame_rasterized_pixels += count;
	_summary_render_stats._frame_rasterized_pixels += count;
}

void GPU::add_fragments(uint64_t count) noexcept
{
	_render_stats._frame_fragments += count;
	_summary_render_stats._frame_fragments += count;
}

void GPU::add_texture_samples(uint64_t count) noexcept
{
	_render_stats._frame_texture_samples += count;
	_summary_render_stats._frame_texture_samples += count;
}

void GPU::reset_render_stats() noexcept
{
	_render_stats = RenderStats{};
	_render_stats._frame_count = 1;
	++_summary_render_stats._frame_count;
}

void GPU::accumulate_draw_stats(uint8_t draw_mode, size_t count) noexcept
{
	++_render_stats._frame_draw_calls;
	_render_stats._frame_vertices += count;
	++_summary_render_stats._frame_draw_calls;
	_summary_render_stats._frame_vertices += count;

	if (draw_mode == MAI_DRAW_TRIANGLES)
	{
		_render_stats._frame_triangles += count / 3;
		_summary_render_stats._frame_triangles += count / 3;
	}
}

void GPU::accumulate_pipeline_stats(const RenderStats& stats) noexcept
{
	_render_stats._frame_rasterized_pixels += stats._frame_rasterized_pixels;
	_render_stats._frame_fragments += stats._frame_fragments;
	_render_stats._frame_texture_samples += stats._frame_texture_samples;
	_render_stats._vertex_ms += stats._vertex_ms;
	_render_stats._primitive_ms += stats._primitive_ms;
	_render_stats._clip_ms += stats._clip_ms;
	_render_stats._perspective_division_ms += stats._perspective_division_ms;
	_render_stats._cull_ms += stats._cull_ms;
	_render_stats._screen_mapping_ms += stats._screen_mapping_ms;
	_render_stats._raster_ms += stats._raster_ms;
	_render_stats._perspective_recover_ms += stats._perspective_recover_ms;
	_render_stats._fragment_ms += stats._fragment_ms;

	_summary_render_stats._frame_rasterized_pixels += stats._frame_rasterized_pixels;
	_summary_render_stats._frame_fragments += stats._frame_fragments;
	_summary_render_stats._frame_texture_samples += stats._frame_texture_samples;
	_summary_render_stats._vertex_ms += stats._vertex_ms;
	_summary_render_stats._primitive_ms += stats._primitive_ms;
	_summary_render_stats._clip_ms += stats._clip_ms;
	_summary_render_stats._perspective_division_ms += stats._perspective_division_ms;
	_summary_render_stats._cull_ms += stats._cull_ms;
	_summary_render_stats._screen_mapping_ms += stats._screen_mapping_ms;
	_summary_render_stats._raster_ms += stats._raster_ms;
	_summary_render_stats._perspective_recover_ms += stats._perspective_recover_ms;
	_summary_render_stats._fragment_ms += stats._fragment_ms;
}

}
