#pragma once

#include <cstdint>

namespace mai
{

struct RenderStats
{
	uint64_t _frame_count{ 0 };
	uint64_t _frame_draw_calls{ 0 };
	uint64_t _frame_triangles{ 0 };
	uint64_t _frame_vertices{ 0 };
	uint64_t _frame_rasterized_pixels{ 0 };
	uint64_t _frame_fragments{ 0 };
	uint64_t _frame_texture_samples{ 0 };

	double _vertex_ms{ 0.0 };
	double _primitive_ms{ 0.0 };
	double _clip_ms{ 0.0 };
	double _perspective_division_ms{ 0.0 };
	double _cull_ms{ 0.0 };
	double _screen_mapping_ms{ 0.0 };
	double _raster_ms{ 0.0 };
	double _perspective_recover_ms{ 0.0 };
	double _fragment_ms{ 0.0 };
};

}
