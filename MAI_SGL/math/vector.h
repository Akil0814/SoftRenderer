#pragma once
#include <iostream>
#include <assert.h>

namespace mai
{
	template<typename T>
	class Vector2;
	template<typename T>
	class Vector3;
	template<typename T>
	class Vector4;

	template<typename T>
	class Vector2
	{
	public:
		Vector2()noexcept : x(0), y(0) {}
		Vector2(T x, T y)noexcept :x(x), y(y) {}
		Vector2(const Vector2<T>& v)noexcept :x(v.x), y(v.y) {}
		explicit Vector2(const Vector3<T>& v)noexcept :x(v.x), y(v.y) {}
		explicit Vector2(const Vector4<T>& v)noexcept :x(v.x), y(v.y) {}

	
		//取得向量某个元素
		[[nodiscard]] const T& operator[](int i) const noexcept
		{
			assert(i >= 0 && i < 2);
			if (i == 0)
				return x;
			return y;
		}

		//给向量某个元素赋值
		T& operator[](int i)noexcept
		{
			assert(i >= 0 && i < 2);

			if (i == 0)
				return x;
			return y;
		}

		//等号运算符重载
		Vector2<T>& operator=(const Vector3<T>& v)noexcept
		{
			x = v.x; y = v.y;
			return *this;
		}

		Vector2<T>& operator=(const Vector4<T>& v)noexcept
		{
			x = v.x; y = v.y;
			return *this;
		}

		//加法
		//v = v1+v2
		[[nodiscard]] Vector2<T> operator+(const Vector2<T>& v) const noexcept
		{
			return Vector2<T>(x + v.x, y + v.y);
		}

		//加法并赋值
		//v += v2
		Vector2<T>& operator+=(const Vector2<T>& v)noexcept
		{
			x += v.x; y += v.y;
			return *this;
		}

		//减法
		[[nodiscard]] Vector2<T> operator-(const Vector2<T>& v) const noexcept
		{
			return Vector2<T>(x - v.x, y - v.y);
		}

		Vector2<T>& operator-=(const Vector2<T>& v)noexcept
		{
			x -= v.x;
			y -= v.y;
			return *this;
		}

		//乘法
		//v = v1 * s
		[[nodiscard]] Vector2<T> operator*(T s) const noexcept
		{
			return Vector2<T>(x * s, y * s);
		}

		//乘法并赋值
		//v *= s
		Vector2<T>& operator*=(T s)noexcept
		{
			x *= s; y *= s;
			return *this;
		}

		//除法
		//v = v1 / f
		//int 除法可能会被截断
		[[nodiscard]] Vector2<T> operator/(T f) const noexcept
		{
			assert(f != 0);
			const float inv = 1.0f / static_cast<float>(f);

			return Vector2(x * inv, y * inv);
		}

		//除法并赋值
		//v /= f
		Vector2<T>& operator/=(T f)noexcept
		{
			assert(f != 0);
			const float inv = 1.0f / static_cast<float>(f);
			x *= inv; y *= inv;
			return *this;
		}

		//取反
		[[nodiscard]] Vector2<T> operator-() const noexcept
		{
			return Vector2<T>(-x, -y);
		}


		void print() const
		{
			std::cout << "Vector2 is:" << std::endl;
			std::cout << "x = " << x << ", y = " << y << std::endl;
		}

	public:
		T x, y;
	};

	template<typename T>
	class Vector3 {
	public:
		Vector3()noexcept :x(0), y(0), z(0) {}
		Vector3(T x, T y, T z)noexcept :x(x), y(y), z(z) {}
		Vector3(const Vector3<T>& v)noexcept :x(v.x), y(v.y), z(v.z) {}
		explicit Vector3(const Vector4<T>& v)noexcept :x(v.x), y(v.y), z(v.z) {}

		[[nodiscard]] const T& operator[](int i) const noexcept
		{
			assert(i >= 0 && i <= 2);

			if (i == 0)
				return x;
			if (i == 1)
				return y;

			return z;
		}

		T& operator[](int i)noexcept
		{
			assert(i >= 0 && i <= 2);

			if (i == 0)
				return x;
			if (i == 1)
				return y;

			return z;
		}

		Vector3<T>& operator=(const Vector2<T>& v)noexcept
		{
			x = v.x;
			y = v.y;
			z = 0;
			return *this;
		}

		Vector3<T>& operator=(const Vector4<T>& v)noexcept
		{
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}

		[[nodiscard]] Vector3<T> operator+(const Vector3<T>& v) const noexcept
		{
			return Vector3<T>(x + v.x, y + v.y, z + v.z);
		}

		Vector3<T>& operator+=(const Vector3<T>& v)noexcept
		{
			x += v.x; y += v.y; z += v.z;
			return *this;
		}

		[[nodiscard]] Vector3<T> operator-(const Vector3<T>& v) const noexcept
		{
			return Vector3<T>(x - v.x, y - v.y, z - v.z);
		}

		Vector3<T>& operator-=(const Vector3<T>& v)noexcept
		{
			x -= v.x; y -= v.y; z -= v.z;
			return *this;
		}

		[[nodiscard]] Vector3<T> operator*(T s) const noexcept
		{
			return Vector3<T>(x * s, y * s, z * s);
		}

		Vector3<T>& operator*=(T s)noexcept
		{
			x *= s; y *= s; z *= s;
			return *this;
		}

		[[nodiscard]] Vector3<T> operator/(T f) const noexcept
		{
			assert(f != 0);
			const float inv = 1.0f / static_cast<float>(f);
			return Vector3<T>(x * inv, y * inv, z * inv);
		}

		Vector3<T>& operator/=(T f)noexcept
		{
			assert(f != 0);
			const float inv = 1.0f / static_cast<float>(f);
			x *= inv; y *= inv; z *= inv;
			return *this;
		}

		[[nodiscard]] Vector3<T>  operator-() const noexcept
		{
			return Vector3<T>(-x, -y, -z);
		}

		void print() const
		{
			std::cout << "Vector3 is:" << std::endl;
			std::cout << "x = " << x << ", y = " << y << ", z = " << z << std::endl;
		}

	public:
		T x, y, z;
	};

	template<typename T>
	class Vector4 {
	public:
		Vector4()noexcept : x(0), y(0), z(0), w(0) {}
		Vector4(T x, T y, T z, T w)noexcept :x(x), y(y), z(z), w(w) {}
		Vector4(const Vector4<T>& v)noexcept :x(v.x), y(v.y), z(v.z), w(v.w) {}

		[[nodiscard]] const T& operator[](int i) const noexcept
		{
			assert(i >= 0 && i <= 3);

			if (i == 0)
				return x;
			if (i == 1)
				return y;
			if (i == 2)
				return z;

			return w;
		}

		T& operator[](int i)noexcept
		{
			assert(i >= 0 && i <= 3);

			if (i == 0)
				return x;
			if (i == 1)
				return y;
			if (i == 2)
				return z;

			return w;
		}

		Vector4<T>& operator=(const Vector2<T>& v)noexcept
		{
			x = v.x;
			y = v.y;
			z = 0;
			w = 0;
			return *this;
		}

		Vector4<T>& operator=(const Vector3<T>& v)noexcept
		{
			x = v.x;
			y = v.y;
			z = v.z;
			w = 0;
			return *this;
		}

		[[nodiscard]] Vector4<T> operator+(const Vector4<T>& v) const noexcept
		{
			return Vector4<T>(x + v.x, y + v.y, z + v.z, w + v.w);
		}

		Vector4<T>& operator+=(const Vector4<T>& v)noexcept
		{
			x += v.x; y += v.y; z += v.z; w += v.w;
			return *this;
		}

		[[nodiscard]] Vector4<T> operator-(const Vector4<T>& v) const noexcept
		{
			return Vector4<T>(x - v.x, y - v.y, z - v.z, w - v.w);
		}

		Vector4<T>& operator-=(const Vector4<T>& v)noexcept
		{
			x -= v.x; y -= v.y; z -= v.z; w -= v.w;
			return *this;
		}

		[[nodiscard]] Vector4<T> operator*(T s) const noexcept
		{
			return Vector4<T>(x * s, y * s, z * s, w * s);
		}

		Vector4<T>& operator*=(T s)noexcept
		{
			x *= s; y *= s; z *= s; w *= s;
			return *this;
		}

		Vector4<T>& operator*=(const Vector3<T>& v)noexcept
		{
			x *= v.x; y *= v.y; z *= v.z;
			return *this;
		}

		[[nodiscard]] Vector4<T> operator/(T f) const noexcept
		{
			assert(f != 0);
			const float inv = 1.0f / static_cast<float>(f);
			return Vector4<T>(x * inv, y * inv, z * inv, w * inv);
		}

		Vector4<T>& operator/=(T f)noexcept
		{
			assert(f != 0);
			const float inv = 1.0f / static_cast<float>(f);
			x *= inv; y *= inv; z *= inv; w *= inv;
			return *this;
		}

		[[nodiscard]] Vector4<T> operator-() const noexcept
		{
			return Vector4<T>(-x, -y, -z, -w);
		}

		void print() const
		{
			std::cout << "Vector4 is:" << std::endl;
			std::cout << "x = " << x << ", y = " << y << ", z = " << z << ", w = " << w << std::endl;
			std::cout << std::endl;
		}

	public:
		T x, y, z, w;
	};

	using vec2f = Vector2<float>;
	using vec2i = Vector2<int>;
	using vec3f = Vector3<float>;
	using vec3i = Vector3<int>;
	using vec4f = Vector4<float>;
	using vec4i = Vector4<int>;
}
