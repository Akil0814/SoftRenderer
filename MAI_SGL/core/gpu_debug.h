#pragma once

namespace mai
{

class VertexArrayObject;
struct RenderStats;

namespace gpu_debug
{

void print_vao(VertexArrayObject& vao);
void print_frame_stats(const RenderStats& stats);
void print_summary_stats(const RenderStats& stats);

}

}
