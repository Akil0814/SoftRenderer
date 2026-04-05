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

	mai::vec2f pv0, pv1, pv2;
	Point result;
	
	for (int i = min_X; i <= max_X; ++i)
	{
		for (int j = min_Y; j <= max_Y; ++j)
		{
			//使用像素格中点
			float px = i + 0.5f;
			float py = j + 0.5f;

			//获取当前点与三角形相关三个向量
			pv0 = mai::vec2f(v0.x - px, v0.y - py);
			pv1 = mai::vec2f(v1.x - px, v1.y - py);
			pv2 = mai::vec2f(v2.x - px, v2.y - py);

			float cross1 = mai::cross(pv0, pv1);
			float cross2 = mai::cross(pv1, pv2);
			float cross3 = mai::cross(pv2, pv0);

			bool negative = cross1 <= 0 && cross2 <= 0 && cross3 <= 0;
			bool positive = cross1 >= 0 && cross2 >= 0 && cross3 >= 0;

			if (negative || positive)
			{
				result.x = i;
				result.y = j;
				interpolant_triangle(v0, v1, v2, result);

				results.push_back(result);
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

void Raster::interpolant_triangle(const Point& v0, const Point& v1, const Point& v2, Point& target)
{
	//重心插值
	
	//计算总面积
	vec2f e1 = mai::vec2f(v1.x - v0.x, v1.y - v0.y);
	vec2f e2 = mai::vec2f(v2.x - v0.x, v2.y - v0.y);
	float area = std::abs(mai::cross(e1, e2));

	if (area <= 1e-6f)
		return;

	//根据三角形内的点进行区域划分
	vec2f pv0 = mai::vec2f(v0.x - target.x, v0.y - target.y);
	vec2f pv1 = mai::vec2f(v1.x - target.x, v1.y - target.y);
	vec2f pv2 = mai::vec2f(v2.x - target.x, v2.y - target.y);

	//计算分区后的面积
	float v0_area = std::abs(mai::cross(pv1, pv2));
	float v1_area = std::abs(mai::cross(pv0, pv2));
	float v2_area = std::abs(mai::cross(pv0, pv1));

	//计算区域权重

	float weight0 = v0_area / area;
	float weight1 = v1_area / area;
	float weight2 = v2_area / area;

	RGBA result;

	RGBA c0 = v0.color;
	RGBA c1 = v1.color;
	RGBA c2 = v2.color;

	result._R = static_cast<float>(c0._R) * weight0 + static_cast<float>(c1._R) * weight1 + static_cast<float>(c2._R) * weight2;
	result._G = static_cast<float>(c0._G) * weight0 + static_cast<float>(c1._G) * weight1 + static_cast<float>(c2._G) * weight2;
	result._B = static_cast<float>(c0._B) * weight0 + static_cast<float>(c1._B) * weight1 + static_cast<float>(c2._B) * weight2;
	result._A = static_cast<float>(c0._A) * weight0 + static_cast<float>(c1._A) * weight1 + static_cast<float>(c2._A) * weight2;

	target.color = result;
}

}


//int max_X = static_cast<int>(std::max(v0.x, std::max(v1.x, v2.x)));
//int min_X = static_cast<int>(std::min(v0.x, std::min(v1.x, v2.x)));
//int max_Y = static_cast<int>(std::max(v0.y, std::max(v1.y, v2.y)));
//int min_Y = static_cast<int>(std::min(v0.y, std::min(v1.y, v2.y)));
//
//Point right, left;
//
//rasterize_line(v0, v1, results);
//rasterize_line(v1, v2, results);
//rasterize_line(v2, v0, results);
//
//std::vector<Point> tmp = results;
//
//while (!tmp.empty())
//{
//	right = tmp.back();
//	tmp.pop_back();
//	for (int i = 0; i < tmp.size(); ++i)
//	{
//		if (tmp[i].x == right.x)
//		{
//			left = tmp[i];
//			std::swap(tmp[i], tmp.back());
//			tmp.pop_back();
//			rasterize_line(right, left, results);
//		}
//	}
//}