#pragma once
#include "data_structures.h"

namespace mai
{

class Clipper
{
public:
	static void do_clip_space(const uint32_t& draw_mode, const std::vector<VsOutput>& primitives, std::vector<VsOutput>& outputs);

private:
	static void sutherland_hodgman(const uint32_t& draw_mode, const std::vector<VsOutput>& primitive, std::vector<VsOutput>& outputs);

	static bool inside(const mai::vec4f& point, const mai::vec4f& plane);

	static VsOutput intersect(const VsOutput& last, const VsOutput& current, const mai::vec4f& plane);
};

}
