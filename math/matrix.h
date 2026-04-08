#pragma once
#include <iostream>
#include <assert.h>
#include "vector.h"

namespace mai
{
	template<typename T>
	class Matrix3x3;

	template<typename T>
	class Matrix4x4;
	
   /* m0 m3 m6
	* m1 m4 m7
	* m2 m5 m8 */
	template<typename T>
	class Matrix3x3
	{
	public:
		Matrix3x3()noexcept = default;
		explicit Matrix3x3(T v)noexcept
		{
			m[0] = v; m[3] = 0; m[6] = 0;
			m[1] = 0; m[4] = v; m[7] = 0;
			m[2] = 0; m[5] = 0; m[8] = v;
		}

		Matrix3x3(
			T m00, T m01, T m02,
			T m10, T m11, T m12,
			T m20, T m21, T m22
		)noexcept
		{
			set(m00, m01, m02,
				m10, m11, m12,
				m20, m21, m22);
		}

		Matrix3x3(const Matrix3x3<T>& src)noexcept
		{
			memcpy((void*)m, (void*)src.m, sizeof(T) * 9);
		}

		Matrix3x3(const Matrix4x4<T>& src)noexcept
		{
			m[0] = src.m[0]; m[3] = src.m[4]; m[6] = src.m[8];
			m[1] = src.m[1]; m[4] = src.m[5]; m[7] = src.m[9];
			m[2] = src.m[2]; m[5] = src.m[6]; m[8] = src.m[10];
		}

		[[nodiscard]] Matrix3x3<T> operator*(const T& s)const noexcept
		{
			Matrix3x3<T> result;

			Vector3<T> col0 = this->get_column(0) * s;
			Vector3<T> col1 = this->get_column(1) * s;
			Vector3<T> col2 = this->get_column(2) * s;

			result.set_column(col0, 0);
			result.set_column(col1, 1);
			result.set_column(col2, 2);

			return result;
		}

		[[nodiscard]] Vector3<T> operator*(const Vector3<T>& v)const noexcept
		{
			return Vector3<T>(
				v.x * m[0] + v.y * m[3] + v.z * m[6],
				v.x * m[1] + v.y * m[4] + v.z * m[7],
				v.x * m[2] + v.y * m[5] + v.z * m[8]
			);
		}

		[[nodiscard]] inline T get(uint32_t row, uint32_t col) const noexcept
		{
			assert(row < 3 && col < 3);
			return m[col * 3 + row];
		}

		inline void set(uint32_t row, uint32_t col, T t) noexcept
		{
			assert(row < 3 && col < 3);
			m[col * 3 + row] = t;
		}

		inline void set(
			T m00, T m01, T m02,
			T m10, T m11, T m12,
			T m20, T m21, T m22
		) noexcept
		{
			m[0] = m00; m[3] = m01; m[6] = m02;
			m[1] = m10; m[4] = m11; m[7] = m12;
			m[2] = m20; m[5] = m21; m[8] = m22;
		}

		Matrix3x3<T>& set_identity() noexcept
		{
			set(
				T(1), T(0), T(0),
				T(0), T(1), T(0),
				T(0), T(0), T(1)
			);

			return *this;
		}

		[[nodiscard]] static Matrix3x3<T> identity() noexcept
		{
			return Matrix3x3<T>(
				T(1), T(0), T(0),
				T(0), T(1), T(0),
				T(0), T(0), T(1)
			);
		}

		[[nodiscard]] Vector3<T> get_column(uint32_t col) const noexcept
		{
			assert(col < 3);
			return Vector3<T>(m[col * 3], m[col * 3 + 1], m[col * 3 + 2]);
		}

		inline void set_column(const Vector3<T>& c_value, uint32_t col) noexcept
		{
			assert(col < 3);
			m[col * 3] = c_value.x; m[col * 3 + 1] = c_value.y; m[col * 3 + 2] = c_value.z;
		}

		void print_matrix()const
		{
			std::cout << "Matrix3x3 is:" << std::endl;
			std::cout << m[0] << "," << m[3] << "," << m[6] << std::endl;
			std::cout << m[1] << "," << m[4] << "," << m[7] << std::endl;
			std::cout << m[2] << "," << m[5] << "," << m[8] << std::endl;
			std::cout << std::endl;
		}

	public:
		T m[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
	};

	
   /* m0 m4 m8	m12
	* m1 m5 m9	m13
	* m2 m6 m10 m14
	* m3 m7 m11	m15 */
	template<typename T>
	class Matrix4x4 {
	public:
		Matrix4x4() noexcept = default;
		explicit Matrix4x4(T v)noexcept
		{
			m[0] = m[5] = m[10] = m[15] = v;
		}
		Matrix4x4(
			T m00, T m01, T m02, T m03,
			T m10, T m11, T m12, T m13,
			T m20, T m21, T m22, T m23,
			T m30, T m31, T m32, T m33
		)noexcept
		{
			set(m00, m01, m02, m03,
				m10, m11, m12, m13,
				m20, m21, m22, m23,
				m30, m31, m32, m33);
		}
		Matrix4x4(const Matrix4x4<T>& src)noexcept
		{
			memcpy((void*)m, (void*)src.m, sizeof(T) * 16);
		}

		[[nodiscard]] Matrix4x4<T> operator*(const T& s) const noexcept
		{
			Matrix4x4<T> result;

			Vector4<T> col0 = this->get_column(0) * s;
			Vector4<T> col1 = this->get_column(1) * s;
			Vector4<T> col2 = this->get_column(2) * s;
			Vector4<T> col3 = this->get_column(3) * s;

			result.set_column(col0, 0);
			result.set_column(col1, 1);
			result.set_column(col2, 2);
			result.set_column(col3, 3);

			return result;
		}

		[[nodiscard]] Vector4<T> operator*(const Vector4<T>& v) const noexcept
		{
			return Vector4<T>(
				v.x * m[0] + v.y * m[4] + v.z * m[8] + v.w * m[12],
				v.x * m[1] + v.y * m[5] + v.z * m[9] + v.w * m[13],
				v.x * m[2] + v.y * m[6] + v.z * m[10] + v.w * m[14],
				v.x * m[3] + v.y * m[7] + v.z * m[11] + v.w * m[15]
			);
		}

		[[nodiscard]] inline T get(uint32_t row, uint32_t col) const noexcept
		{
			assert(row < 4 && col < 4);
			return m[col * 4 + row];
		}

		inline void set(uint32_t row,uint32_t col, T t) noexcept
		{
			assert(row < 4 && col < 4);
			m[col * 4 + row] = t;
		}

		inline void set(
			T m00, T m01, T m02, T m03,
			T m10, T m11, T m12, T m13,
			T m20, T m21, T m22, T m23,
			T m30, T m31, T m32, T m33
		) noexcept
		{
			m[0] = m00; m[4] = m01; m[8] = m02; m[12] = m03;
			m[1] = m10; m[5] = m11; m[9] = m12; m[13] = m13;
			m[2] = m20; m[6] = m21; m[10] = m22; m[14] = m23;
			m[3] = m30; m[7] = m31; m[11] = m32; m[15] = m33;
		}

		Matrix4x4<T>& set_identity() noexcept
		{
			set(
				T(1), T(0), T(0), T(0),
				T(0), T(1), T(0), T(0),
				T(0), T(0), T(1), T(0),
				T(0), T(0), T(0), T(1)
			);
			return *this;
		}

		[[nodiscard]] static Matrix4x4<T> identity() noexcept
		{
			return Matrix4x4<T>(
				T(1), T(0), T(0), T(0),
				T(0), T(1), T(0), T(0),
				T(0), T(0), T(1), T(0),
				T(0), T(0), T(0), T(1)
			);
		}

		[[nodiscard]] inline Vector4<T> get_column(uint32_t col) const noexcept
		{
			assert(col < 4);
			return Vector4<T>(
				m[col * 4],
				m[col * 4 + 1],
				m[col * 4 + 2],
				m[col * 4 + 3]);
		}

		inline void set_column(const Vector4<T>& cvalue, uint32_t col) noexcept
		{
			assert(col < 4);
			m[col * 4] = cvalue.x;
			m[col * 4 + 1] = cvalue.y;
			m[col * 4 + 2] = cvalue.z;
			m[col * 4 + 3] = cvalue.w;
		}

		void print_matrix() const
		{
			std::cout << "Matrix4x4 is:" << std::endl;
			std::cout << m[0] << "," << m[4] << "," << m[8] << "," << m[12] << std::endl;
			std::cout << m[1] << "," << m[5] << "," << m[9] << "," << m[13] << std::endl;
			std::cout << m[2] << "," << m[6] << "," << m[10] << "," << m[14] << std::endl;
			std::cout << m[3] << "," << m[7] << "," << m[11] << "," << m[15] << std::endl;
			std::cout << std::endl;
		}

	public:
		T m[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	};

	using Mat3f = Matrix3x3<float>;
	using Mat4f = Matrix4x4<float>;
}
