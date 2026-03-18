#pragma once

#include<iostream>
#include<vector>
#include<map>
#include<cmath>
#include<assert.h>

#define PI					3.14159265358979323
#define DEG2RAD(theta)		(0.01745329251994329 * (theta))
#define FRACTION(v)			((v) - (int)(v))//返回小数部分

using byte = unsigned char;

struct RGBA
{
	byte _R;
	byte _G;
	byte _B;
	byte _A;

	RGBA(
		byte r = 255,
		byte g = 255,
		byte b = 255,
		byte a = 255)
	{
		_R = r;
		_G = g;
		_B = b;
		_A = a;
	}
};