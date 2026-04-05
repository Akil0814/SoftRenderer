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
		std::vector<Point>& results,
		const Point& v0,
		const Point& v1
	);

	static void interpolant_line(const Point& v0, const Point& v1, Point& target);

};

}
