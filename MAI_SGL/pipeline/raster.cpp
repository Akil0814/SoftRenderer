#include "raster.h"
#include "../math/math.h"

namespace mai
{

	void Raster::rasterize(
		const DrawContext& context,uint8_t draw_mode, const std::vector<VertexShaderOutput>& inputs,
		std::vector<VertexShaderOutput>& results)
	{
		if (draw_mode == MAI_DRAW_LINES)
		{
			for (uint32_t i = 0; i < inputs.size(); i += 2)
			{
				rasterize_line(context,inputs[i], inputs[i + 1], results);
			}
		}

		if (draw_mode == MAI_DRAW_TRIANGLES)
		{
			for (uint32_t i = 0; i < inputs.size(); i += 3)
			{
				rasterize_triangle(context,inputs[i], inputs[i + 1], inputs[i + 2], results);
			}
		}
	}

void Raster::rasterize_line(
	const DrawContext& context, const VertexShaderOutput& v0, const VertexShaderOutput& v1,
	std::vector<VertexShaderOutput>& results
)
{
	//变换到第一象限/第一八分区再还原
	VsOutput start = v0;
	VsOutput end = v1;

	//确保是从小到大
	if (start._position.x > end._position.x)
		std::swap(start, end);

	auto in_scissor_rect = [&context](const VsOutput& point) noexcept
	{
		if (!context._state._enable_scissor_test)
			return true;

		const auto& rect = context._state._scissor_clip_rect;
		return rect.contains(
			static_cast<int>(point._position.x),
			static_cast<int>(point._position.y));
	};

	if (in_scissor_rect(start))
		results.push_back(start);

	//保证y方向也是从小到大
	bool flip_y = false;
	if (start._position.y > end._position.y)
	{
		start._position.y = -start._position.y;
		end._position.y = -end._position.y;
		flip_y = true;
	}

	//保证斜率在0-1之间
	int delta_x = static_cast<int>(end._position.x - start._position.x);
	int delta_y = static_cast<int>(end._position.y - start._position.y);

	bool swap_xy = false;
	if (delta_x < delta_y)
	{
		std::swap(start._position.x, start._position.y);
		std::swap(end._position.x, end._position.y);
		std::swap(delta_x, delta_y);
		swap_xy = true;
	}

	//brensenham
	int current_x = static_cast<int>(start._position.x);
	int current_y = static_cast<int>(start._position.y);

	int result_x = 0;
	int result_y = 0;

	VsOutput current_point;
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

		current_point._position.x = result_x;
		current_point._position.y = result_y;

		interpolant_line(v0, v1, current_point);

		if (in_scissor_rect(current_point))
			results.push_back(current_point);
	}
}

void Raster::rasterize_triangle(
	const DrawContext& context, const VertexShaderOutput& v0, const VertexShaderOutput& v1, const VertexShaderOutput& v2,
	std::vector<VertexShaderOutput>& results)
{
	//raster bbox
	int max_X = static_cast<int>(std::max(v0._position.x, std::max(v1._position.x, v2._position.x)));
	int min_X = static_cast<int>(std::min(v0._position.x, std::min(v1._position.x, v2._position.x)));
	int max_Y = static_cast<int>(std::max(v0._position.y, std::max(v1._position.y, v2._position.y)));
	int min_Y = static_cast<int>(std::min(v0._position.y, std::min(v1._position.y, v2._position.y)));

	if(context._state._enable_scissor_test)
	{
		const auto& rect = context._state._scissor_clip_rect;
		min_X = std::max(min_X, rect._min_x);
   		min_Y = std::max(min_Y, rect._min_y);
    	max_X = std::min(max_X, rect._max_x - 1);
   		max_Y = std::min(max_Y, rect._max_y - 1);
	}

	if (min_X > max_X || min_Y > max_Y)
    	return;

	mai::vec2f pv0, pv1, pv2;
	VsOutput result;
	
	for (int i = min_X; i <= max_X; ++i)
	{
		for (int j = min_Y; j <= max_Y; ++j)
		{
			//使用像素格中点
			float px = i + 0.5f;
			float py = j + 0.5f;

			//获取当前点与三角形相关三个向量
			pv0 = mai::vec2f(v0._position.x - px, v0._position.y - py);
			pv1 = mai::vec2f(v1._position.x - px, v1._position.y - py);
			pv2 = mai::vec2f(v2._position.x - px, v2._position.y - py);

			float cross1 = mai::cross(pv0, pv1);
			float cross2 = mai::cross(pv1, pv2);
			float cross3 = mai::cross(pv2, pv0);

			bool negative = cross1 <= 0 && cross2 <= 0 && cross3 <= 0;
			bool positive = cross1 >= 0 && cross2 >= 0 && cross3 >= 0;

			if (negative || positive)
			{
				result._position.x = i;
				result._position.y = j;
				interpolant_triangle(v0, v1, v2, result);

				results.push_back(result);
			}
		}
	}

}

void  Raster::interpolant_line(const VertexShaderOutput& v0, const VertexShaderOutput& v1,
	VertexShaderOutput& target) noexcept
{
	if (v0._position.x == v1._position.x && v0._position.y == v1._position.y)
	{
		target._color = v0._color;
		return;
	}

	float weight = 1.0f;
	if (v1._position.x != v0._position.x)
		weight = (float)(target._position.x - v0._position.x) / (float)(v1._position.x - v0._position.x);
	else if(v1._position.y!=v0._position.y)
		weight = (float)(target._position.y - v0._position.y) / (float)(v1._position.y - v0._position.y);

	//对于颜色的插值
	target._color = mai::lerp(v0._color, v1._color, weight);

	//对于uv坐标的插值
	target._UV = mai::lerp(v0._UV, v1._UV, weight);
}

void Raster::interpolant_triangle(const VertexShaderOutput& v0, const VertexShaderOutput& v1, const VertexShaderOutput& v2,
	VertexShaderOutput& target) noexcept
{
	//重心插值
	
	//计算总面积
	vec2f e1 = mai::vec2f(v1._position.x - v0._position.x, v1._position.y - v0._position.y);
	vec2f e2 = mai::vec2f(v2._position.x - v0._position.x, v2._position.y - v0._position.y);
	float area = std::abs(mai::cross(e1, e2));

	if (area <= 1e-6f)
		return;

	//根据三角形内的点进行区域划分
	vec2f pv0 = mai::vec2f(v0._position.x - target._position.x, v0._position.y - target._position.y);
	vec2f pv1 = mai::vec2f(v1._position.x - target._position.x, v1._position.y - target._position.y);
	vec2f pv2 = mai::vec2f(v2._position.x - target._position.x, v2._position.y - target._position.y);

	//计算分区后的面积
	float v0_area = std::abs(mai::cross(pv1, pv2));
	float v1_area = std::abs(mai::cross(pv0, pv2));
	float v2_area = std::abs(mai::cross(pv0, pv1));

	//计算区域权重

	float weight0 = v0_area / area;
	float weight1 = v1_area / area;
	float weight2 = v2_area / area;


	//插值1/w，用于透视恢复
	target._inv_w = mai::lerp(v0._inv_w, v1._inv_w, v2._inv_w, weight0, weight1, weight2);

	//插值深度值
	target._position.z = mai::lerp(v0._position.z, v1._position.z, v2._position.z, weight0, weight1, weight2);

	//对于颜色的插值
	target._color = mai::lerp(v0._color, v1._color, v2._color, weight0, weight1, weight2);

	//对于uv坐标的插值
	target._UV = mai::lerp(v0._UV, v1._UV, v2._UV, weight0, weight1, weight2);
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
