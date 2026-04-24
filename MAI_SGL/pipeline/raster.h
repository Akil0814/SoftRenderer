#pragma once
#include "../base.h"
#include "../data_structures.h"
#include "draw_context.h"

#include <vector>

namespace mai
{

class Raster
{
public:
	Raster() {};
	~Raster() {};

public:
	static void rasterize(
		const DrawContext& context,uint8_t draw_mode, const std::vector<VertexShaderOutput>& inputs,
		std::vector<VertexShaderOutput>& results );

private:

	static void rasterize_line(
		const DrawContext& context, const VertexShaderOutput& v0,const VertexShaderOutput& v1,
		std::vector<VertexShaderOutput>& results);

	static void rasterize_triangle(
		const DrawContext& context, const VertexShaderOutput& v0, const VertexShaderOutput& v1, const VertexShaderOutput& v2,
		std::vector<VertexShaderOutput>& results);

	static void interpolant_line(const VertexShaderOutput& v0, const VertexShaderOutput& v1, 
		VertexShaderOutput& target) noexcept;

	static void interpolant_triangle(const VertexShaderOutput& v0, const VertexShaderOutput& v1, const VertexShaderOutput& v2,
		float sample_x, float sample_y, VertexShaderOutput& target) noexcept;

};

}
