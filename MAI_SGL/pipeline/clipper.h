#pragma once
#include "../data_structures.h"
#include <vector>

namespace mai
{

class Clipper
{
public:
	static void do_clip_space(uint32_t draw_mode, const std::vector<VsOutput>& primitives, std::vector<VsOutput>& outputs);
	static bool cull_face(uint32_t front_face, uint32_t call_face, 
		const VsOutput& v0, const VsOutput& v1, const VsOutput& v2);
private:
	static void sutherland_hodgman(uint32_t draw_mode, const std::vector<VsOutput>& primitive, std::vector<VsOutput>& outputs);

	static bool inside(const mai::vec4f& point, const mai::vec4f& plane);

	static VsOutput intersect(const VsOutput& last, const VsOutput& current, const mai::vec4f& plane);
};

}
