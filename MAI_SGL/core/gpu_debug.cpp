#include "gpu.h"

#include <iostream>

#include "gpu_debug.h"

namespace mai
{
namespace gpu_debug
{

void print_vao(VertexArrayObject& vao)
{
	vao.print();
}

void print_frame_stats(const RenderStats& stats)
{
	std::cout
		<< "Frame Stats"
		<< " | Draw Calls: " << stats._frame_draw_calls
		<< " | Triangles: " << stats._frame_triangles
		<< " | Vertices: " << stats._frame_vertices
		<< " | Rasterized Pixels: " << stats._frame_rasterized_pixels
		<< " | Fragments: " << stats._frame_fragments
		<< " | Texture Samples: " << stats._frame_texture_samples
		<< '\n';
}

void print_summary_stats(const RenderStats& stats)
{
	std::cout
		<< "Summary Stats"
		<< " | Draw Calls: " << stats._frame_draw_calls
		<< " | Triangles: " << stats._frame_triangles
		<< " | Vertices: " << stats._frame_vertices
		<< " | Rasterized Pixels: " << stats._frame_rasterized_pixels
		<< " | Fragments: " << stats._frame_fragments
		<< " | Texture Samples: " << stats._frame_texture_samples
		<< '\n';
}

}

void GPU::print_VAO(uint32_t VAO_ID)
{
	auto iter = _VAO_map.find(VAO_ID);
	if (iter != _VAO_map.end())
		gpu_debug::print_vao(*iter->second);
}

void GPU::print_frame_stats()
{
	gpu_debug::print_frame_stats(_render_stats);
}

void GPU::print_summary_stats()
{
	gpu_debug::print_summary_stats(_render_stats);
}

}
