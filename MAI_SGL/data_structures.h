#pragma once
#include "base.h"
#include "math/math.h"

namespace mai
{
	//VAO之中，用于描述属性读取方式的Description
	struct BindingDescription
	{
		uint32_t	_VBO_ID{ 0 };
		size_t		_item_size{ 0 };
		size_t		_stride{ 0 };
		size_t		_offset{ 0 };
	};

	struct VertexShaderOutput
	{
		float _inv_w{ 0 };
		mai::vec4f _position{ 0.0f, 0.0f, 0.0f, 1.0f };
		mai::vec4f _color;//此处颜色改为0.0-1.0之间表达0-255的量
		mai::vec2f _UV;
	};

	struct FragmentShaderOutput
	{
		mai::vec2i _pixel_pos;
		float _depth;
		mai::RGBA _color;//此处使用0-255来进行颜色显示
	};


	using VsOutput = VertexShaderOutput;
	using FsOutput = FragmentShaderOutput;
}

