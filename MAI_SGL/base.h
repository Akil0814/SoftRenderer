#pragma once
#include <cstdint>
#include <cassert>
#include "math/vector.h"

#define MAI_PIXEL_FORMAT_BGRA 1
//#define MAI_PIXEL_FORMAT_RGBA 1


#define MAI_PI					3.14159265358979323f
#define MAI_DEG2RAD(theta)		(0.01745329251994329 * (theta))
#define MAI_FRACTION(v)			((v) - (int)(v))//返回小数部分

#define MAI_TRUE 1
#define MAI_FALSE 0

#define MAI_DRAW_2D 0
#define MAI_DRAW_3D 1

//纹理属性
#define MAI_TEXTURE_WRAP_REPEAT 0
#define MAI_TEXTURE_WRAP_MIRROR 1

//纹理参数类型选项
#define MAI_TEXTURE_FILTER 0
#define MAI_TEXTURE_WRAP_U 1
#define MAI_TEXTURE_WRAP_V 2

#define MAI_TEXTURE_FILTER_NEAREST 0
#define MAI_TEXTURE_FILTER_LINEAR 1

#define MAI_ARRAY_BUFFER 0
#define MAI_ELEMENT_ARRAY_BUFFER 1

#define MAI_DRAW_LINES 0
#define MAI_DRAW_TRIANGLES 1

#define MAI_CULL_FACE 1
#define MAI_DEPTH_TEST 2
#define MAI_BLENDING 3
#define MAI_SCISSOR_TEST 4

//剪裁相关
#define MAI_FRONT_FACE_CW 0
#define MAI_FRONT_FACE_CCW 1

#define MAI_FRONT_FACE 0
#define MAI_BACK_FACE 1


//深度相关
#define MAI_DEPTH_LESS 0
#define MAI_DEPTH_GREATER 1

using byte = std::uint8_t;

namespace mai
{

#if defined(MAI_PIXEL_FORMAT_BGRA)
	struct RGBA
	{
		byte _B;
		byte _G;
		byte _R;
		byte _A;

		constexpr RGBA(byte r = 255, byte g = 255, byte b = 255, byte a = 255)
			: _R(r), _G(g), _B(b), _A(a) {
		}
	};
#elif defined(MAI_PIXEL_FORMAT_RGBA)
	struct RGBA
	{
		byte _R;
		byte _G;
		byte _B;
		byte _A;

		constexpr RGBA(byte r = 255, byte g = 255, byte b = 255, byte a = 255)
			: _R(r), _G(g), _B(b), _A(a) {
		}
	};
#else
#error "Define one pixel format"
#endif

namespace Color
{
	inline constexpr RGBA Black{ 0,   0,   0,   255 };
	inline constexpr RGBA White{ 255, 255, 255, 255 };
	inline constexpr RGBA Red{ 255, 0,   0,   255 };
	inline constexpr RGBA Green{ 0,   255, 0,   255 };
	inline constexpr RGBA Blue{ 0,   0,   255, 255 };
	inline constexpr RGBA Yellow{ 255, 255, 0,   255 };
	inline constexpr RGBA Cyan{ 0,   255, 255, 255 };
	inline constexpr RGBA Magenta{ 255, 0,   255, 255 };
	inline constexpr RGBA Gray{ 128, 128, 128, 255 };
}

}
