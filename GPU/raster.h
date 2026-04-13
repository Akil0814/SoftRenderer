#pragma once
#include "../global/base.h"
#include "data_structures.h"

namespace mai
{

class Raster
{
public:
	Raster() {};
	~Raster() {};

	static void rasterize_line(
		const VertexShaderOutput& v0,const VertexShaderOutput& v1,
		std::vector<VertexShaderOutput>& results);

	static void rasterize_triangle(
		const VertexShaderOutput& v0, const VertexShaderOutput& v1, const VertexShaderOutput& v2,
		std::vector<VertexShaderOutput>& results);

	static void interpolant_line(const VertexShaderOutput& v0, const VertexShaderOutput& v1, 
		VertexShaderOutput& target) noexcept;

	static void interpolant_triangle(const VertexShaderOutput& v0, const VertexShaderOutput& v1, const VertexShaderOutput& v2,
		VertexShaderOutput& target) noexcept;

};

}
