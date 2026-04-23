#pragma once
#include "vector.h"
#include "vector_functions.h"
#include "matrix.h"
#include "../base.h"
#include <cmath>
#include <iostream>

namespace mai
{
	template<typename T>
	[[nodiscard]] Matrix3x3<T> transpose(const Matrix3x3<T>& m) noexcept
	{
		return Matrix3x3<T>(
			m.m[0], m.m[1], m.m[2],
			m.m[3], m.m[4], m.m[5],
			m.m[6], m.m[7], m.m[8]
		);
	}

	template<typename T>
	[[nodiscard]] Matrix4x4<T> transpose(const Matrix4x4<T>& m) noexcept
	{
		return Matrix4x4<T>(
			m.m[0], m.m[1], m.m[2], m.m[3],
			m.m[4], m.m[5], m.m[6], m.m[7],
			m.m[8], m.m[9], m.m[10], m.m[11],
			m.m[12], m.m[13], m.m[14], m.m[15]
		);
	}

	template<typename T>
	[[nodiscard]] Matrix3x3<T> operator * (const Matrix3x3<T>& a, const Matrix3x3<T>& b) noexcept
	{
		Matrix3x3<T> r;

		r.m[0] = a.m[0] * b.m[0] + a.m[3] * b.m[1] + a.m[6] * b.m[2];
		r.m[1] = a.m[1] * b.m[0] + a.m[4] * b.m[1] + a.m[7] * b.m[2];
		r.m[2] = a.m[2] * b.m[0] + a.m[5] * b.m[1] + a.m[8] * b.m[2];

		r.m[3] = a.m[0] * b.m[3] + a.m[3] * b.m[4] + a.m[6] * b.m[5];
		r.m[4] = a.m[1] * b.m[3] + a.m[4] * b.m[4] + a.m[7] * b.m[5];
		r.m[5] = a.m[2] * b.m[3] + a.m[5] * b.m[4] + a.m[8] * b.m[5];

		r.m[6] = a.m[0] * b.m[6] + a.m[3] * b.m[7] + a.m[6] * b.m[8];
		r.m[7] = a.m[1] * b.m[6] + a.m[4] * b.m[7] + a.m[7] * b.m[8];
		r.m[8] = a.m[2] * b.m[6] + a.m[5] * b.m[7] + a.m[8] * b.m[8];

		return r;
	}

	template<typename T>
	[[nodiscard]] Matrix4x4<T> operator * (const Matrix4x4<T>& a, const Matrix4x4<T>& b) noexcept
	{
		Matrix4x4<T> r;

		r.m[0] = a.m[0] * b.m[0] + a.m[4] * b.m[1] + a.m[8] * b.m[2] + a.m[12] * b.m[3];
		r.m[1] = a.m[1] * b.m[0] + a.m[5] * b.m[1] + a.m[9] * b.m[2] + a.m[13] * b.m[3];
		r.m[2] = a.m[2] * b.m[0] + a.m[6] * b.m[1] + a.m[10] * b.m[2] + a.m[14] * b.m[3];
		r.m[3] = a.m[3] * b.m[0] + a.m[7] * b.m[1] + a.m[11] * b.m[2] + a.m[15] * b.m[3];

		r.m[4] = a.m[0] * b.m[4] + a.m[4] * b.m[5] + a.m[8] * b.m[6] + a.m[12] * b.m[7];
		r.m[5] = a.m[1] * b.m[4] + a.m[5] * b.m[5] + a.m[9] * b.m[6] + a.m[13] * b.m[7];
		r.m[6] = a.m[2] * b.m[4] + a.m[6] * b.m[5] + a.m[10] * b.m[6] + a.m[14] * b.m[7];
		r.m[7] = a.m[3] * b.m[4] + a.m[7] * b.m[5] + a.m[11] * b.m[6] + a.m[15] * b.m[7];

		r.m[8] = a.m[0] * b.m[8] + a.m[4] * b.m[9] + a.m[8] * b.m[10] + a.m[12] * b.m[11];
		r.m[9] = a.m[1] * b.m[8] + a.m[5] * b.m[9] + a.m[9] * b.m[10] + a.m[13] * b.m[11];
		r.m[10] = a.m[2] * b.m[8] + a.m[6] * b.m[9] + a.m[10] * b.m[10] + a.m[14] * b.m[11];
		r.m[11] = a.m[3] * b.m[8] + a.m[7] * b.m[9] + a.m[11] * b.m[10] + a.m[15] * b.m[11];

		r.m[12] = a.m[0] * b.m[12] + a.m[4] * b.m[13] + a.m[8] * b.m[14] + a.m[12] * b.m[15];
		r.m[13] = a.m[1] * b.m[12] + a.m[5] * b.m[13] + a.m[9] * b.m[14] + a.m[13] * b.m[15];
		r.m[14] = a.m[2] * b.m[12] + a.m[6] * b.m[13] + a.m[10] * b.m[14] + a.m[14] * b.m[15];
		r.m[15] = a.m[3] * b.m[12] + a.m[7] * b.m[13] + a.m[11] * b.m[14] + a.m[15] * b.m[15];

		return r;
	}

	template<typename T>
	[[nodiscard]] Matrix4x4<T> inverse(const Matrix4x4<T>& src) noexcept
	{
		Matrix4x4<T> result(static_cast<T>(1));

		//计算每个必须的2*2矩阵行列式,下标是左上角到右下角
		T D_22_33 = src.get(2, 2) * src.get(3, 3) - src.get(2, 3) * src.get(3, 2);

		T D_12_23 = src.get(1, 2) * src.get(2, 3) - src.get(1, 3) * src.get(2, 2);
		T D_12_33 = src.get(1, 2) * src.get(3, 3) - src.get(1, 3) * src.get(3, 2);

		T D_21_32 = src.get(2, 1) * src.get(3, 2) - src.get(2, 2) * src.get(3, 1);
		T D_21_33 = src.get(2, 1) * src.get(3, 3) - src.get(2, 3) * src.get(3, 1);

		T D_11_22 = src.get(1, 1) * src.get(2, 2) - src.get(1, 2) * src.get(2, 1);
		T D_11_23 = src.get(1, 1) * src.get(2, 3) - src.get(1, 3) * src.get(2, 1);
		T D_11_32 = src.get(1, 1) * src.get(3, 2) - src.get(1, 2) * src.get(3, 1);
		T D_11_33 = src.get(1, 1) * src.get(3, 3) - src.get(1, 3) * src.get(3, 1);

		T D_02_13 = src.get(0, 2) * src.get(1, 3) - src.get(0, 3) * src.get(1, 2);
		T D_02_23 = src.get(0, 2) * src.get(2, 3) - src.get(0, 3) * src.get(2, 2);
		T D_02_33 = src.get(0, 2) * src.get(3, 3) - src.get(0, 3) * src.get(3, 2);

		T D_01_12 = src.get(0, 1) * src.get(1, 2) - src.get(0, 2) * src.get(1, 1);
		T D_01_13 = src.get(0, 1) * src.get(1, 3) - src.get(0, 3) * src.get(1, 1);
		T D_01_22 = src.get(0, 1) * src.get(2, 2) - src.get(0, 2) * src.get(2, 1);
		T D_01_23 = src.get(0, 1) * src.get(2, 3) - src.get(0, 3) * src.get(2, 1);
		T D_01_32 = src.get(0, 1) * src.get(3, 2) - src.get(0, 2) * src.get(3, 1);
		T D_01_33 = src.get(0, 1) * src.get(3, 3) - src.get(0, 3) * src.get(3, 1);

		//计算伴随阵的每列数据
		Vector4<T> col0, col1, col2, col3;

		/*
		*
		* m5 m9	 m13
		* m6 m10 m14
		* m7 m11 m15
		*/
		col0.x = src.get(1, 1) * D_22_33 - src.get(2, 1) * D_12_33 + src.get(3, 1) * D_12_23;
		col0.y = -(src.get(1, 0) * D_22_33 - src.get(2, 0) * D_12_33 + src.get(3, 0) * D_12_23);
		col0.z = src.get(1, 0) * D_21_33 - src.get(2, 0) * D_11_33 + src.get(3, 0) * D_11_23;
		col0.w = -(src.get(1, 0) * D_21_32 - src.get(2, 0) * D_11_32 + src.get(3, 0) * D_11_22);

		col1.x = -(src.get(0, 1) * D_22_33 - src.get(2, 1) * D_02_33 + src.get(3, 1) * D_02_23);
		col1.y = src.get(0, 0) * D_22_33 - src.get(2, 0) * D_02_33 + src.get(3, 0) * D_02_23;
		col1.z = -(src.get(0, 0) * D_21_33 - src.get(2, 0) * D_01_33 + src.get(3, 0) * D_01_23);
		col1.w = src.get(0, 0) * D_21_32 - src.get(2, 0) * D_01_32 + src.get(3, 0) * D_01_22;

		col2.x = src.get(0, 1) * D_12_33 - src.get(1, 1) * D_02_33 + src.get(3, 1) * D_02_13;
		col2.y = -(src.get(0, 0) * D_12_33 - src.get(1, 0) * D_02_33 + src.get(3, 0) * D_02_13);
		col2.z = src.get(0, 0) * D_11_33 - src.get(1, 0) * D_01_33 + src.get(3, 0) * D_01_13;
		col2.w = -(src.get(0, 0) * D_11_32 - src.get(1, 0) * D_01_32 + src.get(3, 0) * D_01_12);

		col3.x = -(src.get(0, 1) * D_12_23 - src.get(1, 1) * D_02_23 + src.get(2, 1) * D_02_13);
		col3.y = src.get(0, 0) * D_12_23 - src.get(1, 0) * D_02_23 + src.get(2, 0) * D_02_13;
		col3.z = -(src.get(0, 0) * D_11_23 - src.get(1, 0) * D_01_23 + src.get(2, 0) * D_01_13);
		col3.w = src.get(0, 0) * D_11_22 - src.get(1, 0) * D_01_22 + src.get(2, 0) * D_01_12;

		result.set_column(col0, 0);
		result.set_column(col1, 1);
		result.set_column(col2, 2);
		result.set_column(col3, 3);

		//计算行列式
		Vector4<T> row0(result.get(0, 0), result.get(0, 1), result.get(0, 2), result.get(0, 3));
		Vector4<T> colum0 = src.get_column(0);
		T determinant = dot(row0, colum0);

		assert(determinant != 0);

		T one_over_determinant = static_cast<T>(1) / determinant;

		return result * one_over_determinant;
	}

	template<typename T, typename V>
	[[nodiscard]] Matrix4x4<T> scale(const Matrix4x4<T>& src, V x, V y, V z)noexcept
	{
		Matrix4x4<T> result;

		Vector4<T> col0 = src.get_column(0);
		Vector4<T> col1 = src.get_column(1);
		Vector4<T> col2 = src.get_column(2);
		Vector4<T> col3 = src.get_column(3);

		col0 *= x;
		col1 *= y;
		col2 *= z;

		result.set_column(col0, 0);
		result.set_column(col1, 1);
		result.set_column(col2, 2);
		result.set_column(col3, 3);

		return result;
	}

	template<typename T, typename V>
	[[nodiscard]] Matrix4x4<T> translate(const Matrix4x4<T>& src, V x, V y, V z)noexcept
	{
		Matrix4x4<T> result(src);
		Vector4<T> col0 = src.get_column(0);
		Vector4<T> col1 = src.get_column(1);
		Vector4<T> col2 = src.get_column(2);
		Vector4<T> col3 = src.get_column(3);

		Vector4<T> dstCol3 = col0 * x + col1 * y + col2 * z + col3;
		result.set_column(dstCol3, 3);

		return result;
	}

	template<typename T, typename V>
	[[nodiscard]] Matrix4x4<T> translate(const Matrix4x4<T>& src, const Vector3<V>& v)noexcept
	{
		return translate(src, v.x, v.y, v.z);
	}

	template<typename T>
	[[nodiscard]] Matrix4x4<T> rotate(const Matrix4x4<T>& src, float angle, const Vector3<T>& v)
	{
		T const c = std::cos(angle);
		T const s = std::sin(angle);

		Vector3<T> axis = normalize(v);
		Vector3<T> temp((T(1) - c) * axis);

		Matrix4x4<T> Rotate = Matrix4x4<T>::identity();
		Rotate.set(0, 0, c + temp[0] * axis[0]);
		Rotate.set(1, 0, temp[0] * axis[1] + s * axis[2]);
		Rotate.set(2, 0, temp[0] * axis[2] - s * axis[1]);

		Rotate.set(0, 1, temp[1] * axis[0] - s * axis[2]);
		Rotate.set(1, 1, c + temp[1] * axis[1]);
		Rotate.set(2, 1, temp[1] * axis[2] + s * axis[0]);

		Rotate.set(0, 2, temp[2] * axis[0] + s * axis[1]);
		Rotate.set(1, 2, temp[2] * axis[1] - s * axis[0]);
		Rotate.set(2, 2, c + temp[2] * axis[2]);

		//接下来计算 src * rotate
		Vector4<T> rCol0 = Rotate.get_column(0);
		Vector4<T> rCol1 = Rotate.get_column(1);
		Vector4<T> rCol2 = Rotate.get_column(2);
		Vector4<T> rCol3 = Rotate.get_column(3);

		Vector4<T> srcCol0 = src.get_column(0);
		Vector4<T> srcCol1 = src.get_column(1);
		Vector4<T> srcCol2 = src.get_column(2);
		Vector4<T> srcCol3 = src.get_column(3);

		Vector4<T> col0 = srcCol0 * rCol0[0] + srcCol1 * rCol0[1] + srcCol2 * rCol0[2];
		Vector4<T> col1 = srcCol0 * rCol1[0] + srcCol1 * rCol1[1] + srcCol2 * rCol1[2];
		Vector4<T> col2 = srcCol0 * rCol2[0] + srcCol1 * rCol2[1] + srcCol2 * rCol2[2];
		Vector4<T> col3 = srcCol3;

		Matrix4x4<T> result(src);

		result.set_column(col0, 0);
		result.set_column(col1, 1);
		result.set_column(col2, 2);
		result.set_column(col3, 3);

		return result;
	}

	//正交投影函数
	template<typename T>
	[[nodiscard]] Matrix4x4<T> orthographic(T left, T right, T bottom, T top, T near, T far)noexcept
	{
		Matrix4x4<T> result(static_cast<T>(1));

		result.set(0, 0, static_cast<T>(2) / (right - left));
		result.set(0, 3, -(right + left) / (right - left));
		result.set(1, 1, static_cast<T>(2) / (top - bottom));
		result.set(1, 3, -(top + bottom) / (top - bottom));
		result.set(2, 2, -static_cast<T>(2) / (far - near));
		result.set(2, 3, -(far + near) / (far - near));

		return result;
	}

	//透视投影函数
	//这里的fov是y方向的fov
	template<typename T>
	[[nodiscard]] Matrix4x4<T> perspective(T fovy, T aspect, T n, T f)noexcept
	{
		T const tanHalfFovy = std::tan(static_cast<float>(MAI_DEG2RAD(fovy / static_cast<T>(2))));

		Matrix4x4<T> result(static_cast<T>(0));
		result.set(0, 0, static_cast<T>(1) / (aspect * tanHalfFovy));
		result.set(1, 1, static_cast<T>(1) / (tanHalfFovy));
		result.set(2, 2, -(f + n) / (f - n));
		result.set(2, 3, -static_cast<T>(2) * f * n / (f - n));
		result.set(3, 2, -static_cast<T>(1));

		return result;
	}

	//屏幕空间变换函数
	template<typename T>
	[[nodiscard]] Matrix4x4<T> screen_matrix(uint32_t width, uint32_t height)noexcept
	{
		Matrix4x4<T> result(static_cast<T>(1));

		//x
		result.set(0, 0, static_cast<T>(width) / static_cast<T>(2));
		result.set(0, 3, static_cast<T>(width) / static_cast<T>(2));

		//y
		result.set(1, 1, static_cast<T>(height) / static_cast<T>(2));
		result.set(1, 3, static_cast<T>(height) / static_cast<T>(2));

		//z
		result.set(2, 2, static_cast<T>(0.5));
		result.set(2, 3, static_cast<T>(0.5));

		return result;
	}


	template<typename T, typename V>
	[[nodiscard]] Matrix4x4<T> look_at(const Vector3<V>& eye, const Vector3<V>& center, const Vector3<V>& top)
	{
		Vector3<V>& f = normalize(center - eye);
		Vector3<V>& r = normalize(cross(f, top));
		Vector3<V>& u = normalize(cross(r, f));

		Matrix4x4<T> result(static_cast<T>(1));
		//第一行
		result.set(0, 0, r.x);
		result.set(0, 1, r.y);
		result.set(0, 2, r.z);
		result.set(0, 3, -dot(r, eye));

		//第二行
		result.set(1, 0, u.x);
		result.set(1, 1, u.y);
		result.set(1, 2, u.z);
		result.set(1, 3, -dot(u, eye));

		//第三行
		result.set(2, 0, -f.x);
		result.set(2, 1, -f.y);
		result.set(2, 2, -f.z);
		result.set(2, 3, dot(f, eye));

		return result;
	}
}
