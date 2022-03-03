#pragma once

#include <string>
#include "glm/glm.hpp"
#include "fpm/fixed.hpp"
#include "fpm/math.hpp"
#include "core/utility/Log.h"
//https://github.com/MikeLankamp/fpm/blob/master/docs/index.md
//https://en.wikipedia.org/wiki/Fixed-point_arithmetic
#define CUSTOM_FIXED
#ifndef CUSTOM_FIXED
typedef fpm::fixed<std::int32_t, std::int64_t, 8> f32;
typedef f32 fixed;
#else
namespace HBE {
	template<typename T, int32_t B>
	struct fixed;

	typedef fixed<int32_t, 8> f32;
	typedef fixed<int64_t, 32> f64;

	template<typename T, int32_t B>
	struct fixed {

		static const T SCALE;
		static const fixed<T, B> EPSILON;
		static const fixed<T, B> ONE;
		static const fixed<T, B> HALF;
		static const fixed<T, B> QUARTER;
		static const fixed<T, B> PI;
		static const fixed<T, B> TWO_PI;
		static const fixed<T, B> HALF_PI;
		static const fixed<T, B> E;
		T value = 0;

		fixed() {
			this->value = 0;
		}

		fixed(const fixed<T, B> &other) {
			this->value = other.value;
		}

		template<uint32_t B1, typename std::enable_if<(B > B1)>::type * = nullptr>
		fixed(const fixed<T, B1> &f) {

			value = static_cast<T>(f.value << (B - B1));
		}

		template<uint32_t B1, typename std::enable_if<(B <= B1)>::type * = nullptr>
		fixed(const fixed<T, B1> &f) {
			value = static_cast<T>(f.value >> (B1 - B));
		}

		fixed(T value) {
			this->value = value << B;
		}


		static fixed<T, B> raw(T value) {
			fixed<T, B> temp = fixed<T, B>(T{0});
			temp.value = value;
			return temp;
		}

		explicit fixed(float value) {
			this->value = static_cast<T>(value * static_cast<float>(SCALE));
		}

		explicit fixed(double value) {
			this->value = static_cast<T>(value * static_cast<double>(SCALE));
		}

		float toFloat() const {
			return static_cast<float>(value) / static_cast<float>(SCALE);
		}

		explicit operator float() const { return toFloat(); }

		T rounded() {
			return value >> B;
		}

		std::string toString() const {
			return std::to_string(toFloat());
		}

		std::string toFractionString() {
			return std::to_string(value) + "/" + std::to_string(SCALE);
		}

		fixed<T, B> setRaw(T value) {
			this->value = value;
			return *this;
		}

		fixed<T, B> mod(const fixed<T, B> &other) {
			return fixed<T, B>::raw(value % other.value);
		}

		bool operator>(const fixed<T, B> &other) const {

			return value > other.value;
		}

		bool operator<(const fixed<T, B> &other) const {
			return value < other.value;
		}

		bool operator>=(const fixed<T, B> &other) const {

			return value >= other.value;
		}

		bool operator<=(const fixed<T, B> &other) const {
			return value <= other.value;
		}

		fixed<T, B> operator*(const fixed<T, B> &other) const {
			fixed<T, B> temp;
			temp.value = (static_cast<int64_t>(value) * static_cast<int64_t>(other.value)) >> B;

			return temp;
		}

		void operator*=(const fixed<T, B> &other) {
			value = (static_cast<int64_t>(value) * static_cast<int64_t>(other.value)) >> B;
		}

		fixed<T, B> operator/(const fixed<T, B> &other) const {
			fixed<T, B> temp;
			temp.value = (static_cast<int64_t>(value) << B) / static_cast<int64_t>(other.value);
			return temp;
		}

		void operator/=(const fixed<T, B> &other) {
			value = ((value << B) / other.value);
		}

		fixed operator+(const fixed<T, B> &other) const {
			fixed<T, B> temp;
			temp.value = value + other.value;
			return temp;
		}

		void operator+=(const fixed<T, B> &other) {
			value += other.value;
		}

		fixed operator-(const fixed<T, B> &other) const {
			fixed<T, B> temp;
			temp.value = value - other.value;
			return temp;
		}

		void operator-=(const fixed<T, B> &other) {
			value -= other.value;
		}

		void operator++() {
			value += SCALE;
		}

		bool operator==(const fixed<T, B> &other) const {
			return value == other.value;
		}

		bool operator!=(const fixed<T, B> &other) const {
			return value != other.value;
		}


		static fixed<T, B> sin(fixed<T, B> x) {
			// This sine uses a fifth-order curve-fitting approximation originally
			// described by Jasper Vijn on coranac.com which has a worst-case
			// relative error of 0.07% (over [-pi:pi]).
			using fixed_t = fixed<T, B>;

			// Turn x from [0..2*PI] domain into [0..4] domain


			x = x.mod(TWO_PI);
			x /= HALF_PI;

			// Take x modulo one rotation, so [-4..+4].
			if (x.value < 0) {
				x += fixed_t(T{4});
			}

			int sign = +1;
			if (x > fixed_t(T{2})) {
				// Reduce domain to [0..2].
				sign = -1;
				x -= fixed_t(T{2});
			}

			if (x > fixed_t(T{1})) {
				// Reduce domain to [0..1].
				x = fixed_t(T{2}) - x;
			}

			const fixed_t x2 = x * x;
			return fixed_t(sign) * x * (fixed_t::PI - x2 * (fixed_t::TWO_PI - 5 - x2 * (fixed_t::PI - 3))) / 2;
		}

		fixed<T, B> sqrt() {
			if (value == 0) {
				Log::warning("Fixed point sqrt of 0");
				return fixed<T, B>(0);
			}

			int64_t num = static_cast<int64_t>(value) << B;
			int64_t res = 0;
			unsigned long index;

#if defined(__GNUC__) || defined(__clang__)
			index= sizeof(T) * 8 - 1 - __builtin_clzll(value);
#elif defined(_WIN64)
			_BitScanReverse64(&index, value);
#endif

			for (int64_t bit = int64_t{1} << ((index + B) / 2 * 2); bit != 0; bit >>= 2) {
				const T val = res + bit;
				res >>= 1;
				if (num >= val) {
					num -= val;
					res += bit;
				}
			}

			if (num > res) {
				res++;
			}

			return fixed<T, B>::raw(res);

		}

	};

	template<typename T, int32_t B> const T fixed<T, B>::SCALE = T{1} << B;
	template<typename T, int32_t B> const fixed<T, B> fixed<T, B>::EPSILON = fixed<T, B>::raw(T{1});
	template<typename T, int32_t B> const fixed<T, B> fixed<T, B>::ONE = fixed<T, B>(T{1});
	template<typename T, int32_t B> const fixed<T, B> fixed<T, B>::HALF = fixed<T, B>(T{1}) / fixed<T, B>(T{2});
	template<typename T, int32_t B> const fixed<T, B> fixed<T, B>::QUARTER = fixed<T, B>(T{1}) / fixed<T, B>(T{4});
	template<typename T, int32_t B> const fixed<T, B> fixed<T, B>::PI = fixed<T, B>(fixed<int64_t, 61>::raw(static_cast<int64_t>(7244019458077122842ll)));
	template<typename T, int32_t B> const fixed<T, B> fixed<T, B>::TWO_PI = PI * fixed<T, B>(T{2});
	template<typename T, int32_t B> const fixed<T, B> fixed<T, B>::HALF_PI = PI / fixed<T, B>(T{2});;
	template<typename T, int32_t B> const fixed<T, B> fixed<T, B>::E = fixed<T, B>(fixed<int64_t, 61>::raw(static_cast<int64_t>(6267931151224907085ll)));
#endif

	template<typename T>
	struct fixed2 {
		T x, y;

		fixed2() : x(0), y(0) {

		}

		fixed2(T x, T y) : x(x), y(y) {

		}

		static T dot(fixed2 &f1, fixed2 &f2) {
			return (f1.x * f2.x) + (f1.y * f2.y);
		}

		T dot(fixed2 &other) {
			return fixed2::dot(*this, other);
		}

		fixed2 normalized() {
			T lenght = magnitude();
			return fixed2(x / lenght,
						  y / lenght);
		}

		T magnitude() {
			return ((x * x) + (y * y)).sqrt();
		}

		void normalize() {
			T lenght = magnitude();

			x /= lenght;
			y /= lenght;
		}

		std::string toString() {
			return "(" + x.toString() + "," + y.toString() + ")";
		}

		glm::vec2 toVec2() {
			return glm::vec2(static_cast<float>(x), static_cast<float>(y));
		}

		T &operator[](size_t i) {
			switch (i) {
				case 0:
					return x;
				case 1:
					return y;
			}
		}

		fixed2 operator*(const fixed2 &other) const {
			return fixed2(x * other.x,
						  y * other.y);
		}

		fixed2 operator*(const T &other) const {
			fixed2(x * other,
				   y * other);
		}

		void operator*=(const fixed2 &other) {
			x *= other.x;
			y *= other.y;
		}

		void operator*=(const T &other) {
			x *= other;
			y *= other;
		}

		fixed2 operator/(const fixed2 &other) const {
			fixed2(x / other.x,
				   y / other.y);
		}

		fixed2 operator/(const T &other) const {
			fixed2(x / other,
				   y / other);
		}

		void operator/=(const fixed2 &other) {
			x /= other.x;
			y /= other.y;
		}

		void operator/=(const T &other) {
			x /= other;
			y /= other;
		}

		fixed2 operator+(const fixed2 &other) const {
			return fixed2(x + other.x,
						  y + other.y);
		}

		fixed2 operator+(const T &other) const {
			fixed2(x + other,
				   y + other);
		}

		void operator+=(const fixed2 &other) {
			x += other.x;
			y += other.y;
		}

		void operator+=(const T &other) {
			x += other;
			y += other;
		}

		fixed2 operator-(const fixed2 &other) const {
			return fixed2(x - other.x,
						  y - other.y);
		}

		fixed2 operator-(const T &other) const {
			fixed2(x - other,
				   y - other);
		}

		void operator-=(const fixed2 &other) {
			x -= other.x;
			y -= other.y;
		}

		void operator-=(const T &other) {
			x -= other;
			y -= other;
		}

		bool operator==(const fixed2 &other) const {
			return x == other.x && y == other.y;
		}

		bool operator!=(const fixed2 &other) const {
			return x != other.x || y != other.y;
		}

	};

	template<typename T>
	struct fixed3 {
		T x, y, z;


		fixed3() : x(0), y(0), z(0) {

		}

		fixed3(T x, T y, T z) : x(x), y(y), z(z) {

		}

		static T dot(fixed3 &f1, fixed3 &f2) {
			return (f1.x * f2.x) + (f1.y * f2.y) + (f1.z * f2.z);
		}

		T dot(fixed3 &other) {
			return fixed3::dot(*this, other);
		}

		std::string toString() {
			return "(" + x.toString() + "," + y.toString() + "," + z.toString() + ")";
		}

		fixed3 normalized() {
			T mag = magnitude();
			return fixed3(x / mag,
						  y / mag,
						  z / mag);
		}

		T pow2Magnitude() {
			return T((x * x) + (y * y) + (z * z));
		}

		T magnitude() {
			return T(pow2Magnitude()).sqrt();
		}

		void normalize() {
			T lenght = magnitude();

			x /= lenght;
			y /= lenght;
			z /= lenght;
		}


		glm::vec3 toVec3() {
			return glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
		}

		T &operator[](size_t i) {
			switch (i) {
				case 0:
					return x;
				case 1:
					return y;
				case 2:
					return z;
			}
		}

		fixed3 operator*(const fixed3 &other) const {
			return fixed3(x * other.x,
						  y * other.y,
						  z * other.z);
		}

		fixed3 operator*(const T &other) const {
			fixed3(x * other,
				   y * other,
				   z * other);
		}

		void operator*=(const fixed3 &other) {
			x *= other.x;
			y *= other.y;
			z *= other.z;
		}

		void operator*=(const T &other) {
			x *= other;
			y *= other;
			z *= other;
		}

		fixed3 operator/(const fixed3 &other) const {
			return fixed3(x / other.x,
						  y / other.y,
						  z / other.z);
		}

		fixed3 operator/(const T &other) const {
			fixed3(x / other,
				   y / other,
				   z / other);
		}

		void operator/=(const fixed3 &other) {
			x /= other.x;
			y /= other.y;
			z /= other.z;
		}

		void operator/=(const T &other) {
			x /= other;
			y /= other;
			z /= other;
		}

		fixed3 operator+(const fixed3 &other) const {
			return fixed3(x + other.x,
						  y + other.y,
						  z + other.z);
		}

		fixed3 operator+(const T &other) const {
			fixed3(x + other,
				   y + other,
				   z + other);
		}

		void operator+=(const fixed3 &other) {
			x += other.x;
			y += other.y;
			z += other.z;
		}

		void operator+=(const T &other) {
			x += other;
			y += other;
			z += other;
		}

		fixed3 operator-(const fixed3 &other) const {
			return fixed3(x - other.x,
						  y - other.y,
						  z - other.z);
		}

		fixed3 operator-(const T &other) const {
			fixed3(x - other,
				   y - other,
				   z - other);
		}

		void operator-=(const fixed3 &other) {
			x -= other.x;
			y -= other.y;
			z -= other.z;
		}

		void operator-=(const T &other) {
			x -= other;
			y -= other;
			z -= other;
		}

		bool operator==(const fixed3 &other) const {
			return x == other.x && y == other.y && z == other.z;
		}

		bool operator!=(const fixed3 &other) const {
			return x != other.x || y != other.y || z != other.z;
		}
	};

}