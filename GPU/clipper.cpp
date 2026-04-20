#include "clipper.h"
#include "../math/math.h"

namespace mai
{

void Clipper::do_clip_space(const uint32_t& draw_mode, const std::vector<VsOutput>& primitives, std::vector<VsOutput>& outputs)
{
	outputs.clear();

	if (draw_mode == MAI_DRAW_TRIANGLES)
	{
		std::vector<VsOutput> primitive;
		std::vector<VsOutput> results;

		for (uint32_t i = 0; i < primitives.size(); i += 3)
		{
			primitive.clear();
			results.clear();
			auto start = primitives.begin() + i;
			auto end = primitives.begin() + i + 3;
			primitive.assign(start, end);

			Clipper::sutherland_hodgman(draw_mode, primitive, results);

			if (results.empty())
				continue;

			//进行三角形缝补
			for (uint32_t c = 0; c < results.size() - 2; ++c)
			{
				outputs.push_back(results[0]);
				outputs.push_back(results[c + 1]);
				outputs.push_back(results[c + 2]);
			}
		}
	}
	else if (draw_mode == MAI_DRAW_LINES)
	{
		std::vector<VsOutput> primitive;
		std::vector<VsOutput> results;

		for (uint32_t i = 0; i < primitives.size(); i += 2)
		{
			primitive.clear();
			results.clear();
			auto start = primitives.begin() + i;
			auto end = primitives.begin() + i + 2;
			primitive.assign(start, end);
			Clipper::sutherland_hodgman(draw_mode, primitive, results);

			outputs.push_back(results[0]);
			outputs.push_back(results[1]);
		}
	}
}


void Clipper::sutherland_hodgman(const uint32_t& draw_mode, const std::vector<VsOutput>& primitive, std::vector<VsOutput>& outputs)
{
	assert(outputs.size() == 0);

	std::vector<mai::vec4f> clipPlanes =
	{
		//judge w > 0
		mai::vec4f(0.0f, 0.0f, 0.0f, 1.0f),
		//near
		mai::vec4f(0.0f, 0.0f, 1.0f, 1.0f),
		//far
		mai::vec4f(0.0f, 0.0f, -1.0f, 1.0f),
		//left
		mai::vec4f(1.0f, 0.0f, 0.0f, 1.0f),
		//right
		mai::vec4f(-1.0f, 0.0f, 0.0f, 1.0f),
		//top
		mai::vec4f(0.0f, -1.0f, 0.0f, 1.0f),
		//bottom
		mai::vec4f(0.0f, 1.0f, 0.0f, 1.0f)
	};

	//Sutherland-Hodgman algorithm
	outputs = primitive;
	std::vector<VsOutput> inputs;

	//遍历每一个平面进行检查
	for (uint32_t i = 0; i < clipPlanes.size(); ++i) {
		//用上一次的输出作为输入点集
		inputs = outputs;
		outputs.clear();

		//遍历输入的每一个点
		for (uint32_t p = 0; p < inputs.size(); ++p) {
			//取出点P
			auto current = inputs[p];
			//取出点S
			auto last = inputs[(p + inputs.size() - 1) % inputs.size()];

			//1 判定P点在内部,即3/4情况
			if (inside(current._position, clipPlanes[i])) {
				//这里加了限制因素，线条如果到了最后一个顶点，不准回头计算交点
				if (draw_mode != MAI_DRAW_LINES || p != inputs.size() - 1) {

					//2 判定S点不在内部，情况3
					if (!inside(last._position, clipPlanes[i]))
					{
						//求交点I
						VsOutput intersectPoint = intersect(last, current, clipPlanes[i]);

						//按照情况3输出I
						outputs.push_back(intersectPoint);
					}
				}

				//P点只要在内部，都会输出（情况3/4）
				outputs.push_back(current);
			}
			//P点不在内部，即1/2情况
			else {
				// 这里加了限制因素，线条如果到了最后一个顶点，不准回头计算交点
				if (draw_mode != MAI_DRAW_LINES || p != inputs.size() - 1) {

					//S点在内部,情况2,输出交点I
					if (inside(last._position, clipPlanes[i]))
					{
						auto intersectPoint = intersect(last, current, clipPlanes[i]);
						outputs.push_back(intersectPoint);
					}
				}
				//S点不在内部，情况1，无输出
			}
		}
	}
}

bool Clipper::inside(const mai::vec4f& point, const mai::vec4f& plane)
{
	return mai::dot(point, plane) >= 0.0f;
}

VsOutput Clipper::intersect(const VsOutput& last, const VsOutput& current, const mai::vec4f& plane)
{
	VsOutput output;

	float distanceLast = mai::dot(last._position, plane);
	float distanceCurrent = mai::dot(current._position, plane);
	float weight = distanceLast / (distanceLast - distanceCurrent);

	output._position = mai::lerp(last._position, current._position, weight);
	output._color = mai::lerp(last._color, current._color, weight);
	output._UV = mai::lerp(last._UV, current._UV, weight);

	return output;
}

}
