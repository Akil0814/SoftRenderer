#include "raster.h"
#include "../math/math.h"

namespace mai
{

void Raster::rasterize_line(
	const Point& v0,const Point& v1,
	std::vector<Point>& results
)
{
	//变换到第一象限/第一八分区再还原

	Point start = v0;
	Point end = v1;

	//确保是从小到大
	if (start.x > end.x)
		std::swap(start,end);

	results.push_back(start);

	//保证y方向也是从小到大
	bool flip_y = false;
	if (start.y > end.y)
	{
		start.y = -start.y;
		end.y = -end.y;
		flip_y = true;
	}

	//保证斜率在0-1之间
	int delta_x = static_cast<int>(end.x - start.x);
	int delta_y = static_cast<int>(end.y - start.y);

	bool swap_xy = false;
	if (delta_x < delta_y)
	{
		std::swap(start.x, start.y);
		std::swap(end.x, end.y);
		std::swap(delta_x, delta_y);
		swap_xy = true;
	}

	//brensenham
	int current_x = static_cast<int>(start.x);
	int current_y = static_cast<int>(start.y);

	int result_x = 0;
	int result_y = 0;

	Point current_point;
	int p = 2 * delta_y - delta_x;

	for (int i = 0; i < delta_x; ++i)
	{
		if (p >= 0)
		{
			current_y += 1;
			p -= 2 * delta_x;
		}

		current_x += 1;
		p += 2 * delta_y;

		//处理新XY,flip and swap
		result_x = current_x;
		result_y = current_y;
		if (swap_xy)
			std::swap(result_x, result_y);

		if (flip_y)
			result_y = -result_y;

		current_point.x = result_x;
		current_point.y = result_y;

		interpolant_line(v0, v1, current_point);

		results.push_back(current_point);
	}
}

void Raster::rasterize_triangle(
	const Point& v0, const Point& v1, const Point& v2,
	std::vector<Point>& results)
{
	int max_X = static_cast<int>(std::max(v0.x, std::max(v1.x, v2.x)));
	int min_X = static_cast<int>(std::min(v0.x, std::min(v1.x, v2.x)));
	int max_Y = static_cast<int>(std::max(v0.y, std::max(v1.y, v2.y)));
	int min_Y = static_cast<int>(std::min(v0.y, std::min(v1.y, v2.y)));

	Point right,left;

	rasterize_line(v0, v1, results);
	rasterize_line(v1, v2, results);
	rasterize_line(v2, v0,results);

	std::vector<Point> tmp = results;

	while(!tmp.empty())
	{
		right = tmp.back();
		tmp.pop_back();
		for (int i = 0; i < tmp.size(); ++i)
		{
			if (tmp[i].x == right.x)
			{
				left = tmp[i];
				std::swap(tmp[i], tmp.back());
				tmp.pop_back();
				rasterize_line(right, left, results);
			}
		}
	}
}

void  Raster::interpolant_line(const Point& v0, const Point& v1, Point& target)
{
	if (v0.x == v1.x && v0.y == v1.y)
	{
		target.color = v0.color;
		return;
	}

	float weight = 1.0f;
	if (v1.x != v0.x)
		weight = (float)(target.x - v0.x) / (float)(v1.x - v0.x);
	else if(v1.y!=v0.y)
		weight = (float)(target.y - v0.y) / (float)(v1.y - v0.y);

	RGBA result;
	result._R = static_cast<byte>(static_cast<float>(v1.color._R) * weight + (1.0f - weight) * static_cast<float>(v0.color._R));
	result._G = static_cast<byte>(static_cast<float>(v1.color._G) * weight + (1.0f - weight) * static_cast<float>(v0.color._G));
	result._B = static_cast<byte>(static_cast<float>(v1.color._B) * weight + (1.0f - weight) * static_cast<float>(v0.color._B));
	result._A = static_cast<byte>(static_cast<float>(v1.color._A) * weight + (1.0f - weight) * static_cast<float>(v0.color._A));

	target.color = result;
}

}
