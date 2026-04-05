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


};

}
