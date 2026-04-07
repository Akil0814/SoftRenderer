#pragma once
#include "../global/base.h"

namespace mai
{

class Raster
{
public:
	Raster() {};
	~Raster() {};

	static void rasterize_line(
		const Point& v0,const Point& v1,
		std::vector<Point>& results);

	static void rasterize_triangle(
		const Point& v0, const Point& v1, const Point& v2, 
		std::vector<Point>& results);

	static void interpolant_line(const Point& v0, const Point& v1, Point& target);

	static void interpolant_triangle(const Point& v0, const Point& v1, const Point& v2, Point& target);


	//独立的lerp函数
	static RGBA lerpRGBA(const RGBA& c0, const RGBA& c1, float weight);

	static RGBA lerpRGBA(
		const RGBA& c0, const RGBA& c1, const RGBA& c2,
		float weight0, float weight1, float weight2);

	static mai::vec2f lerpUV(
		const vec2f& uv0, const vec2f& uv1, const vec2f& uv2, 
		float weight0, float weight1, float weight2);

};

}
