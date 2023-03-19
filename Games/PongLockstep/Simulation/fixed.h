#pragma once

#include "fpm/fixed.hpp"
#include "fpm/math.hpp"
#include "glm/glm.hpp"

namespace locknet {
	typedef fpm::fixed_16_16 fix16;
	typedef fpm::fixed_24_8 fix8;
	typedef fpm::fixed_8_24 fix24;

	template<typename T>
	struct vec2fixed {
		T x, y;

		vec2fixed() : x(0), y(0) {

		}

		vec2fixed(uint32_t x, uint32_t y) : x(T(x)), y(T(y)) {

		}

		vec2fixed(int32_t x, int32_t y) : x(T(x)), y(T(y)) {

		}

		vec2fixed(T x, T y) : x(x), y(y) {

		}

		static T dot(vec2fixed<T> &f1, vec2fixed<T> &f2) {
			return (f1.x * f2.x) + (f1.y * f2.y);
		}

		T dot(vec2fixed<T> &other) {
			return vec2fixed::dot(*this, other);
		}

		vec2fixed<T> normalized() {
			T lenght = magnitude();
			return vec2fixed<T>(x / lenght,
								y / lenght);
		}

		T magnitude() {
			return sqrt((x * x) + (y * y));
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

		vec2fixed<T> operator*(const vec2fixed<T> &other) const {
			return vec2fixed<T>(x * other.x,
								y * other.y);
		}

		vec2fixed<T> operator*(const T &other) const {
			return vec2fixed<T>(x * other,
								y * other);
		}

		void operator*=(const vec2fixed<T> &other) {
			x *= other.x;
			y *= other.y;
		}

		void operator*=(const T &other) {
			x *= other;
			y *= other;
		}

		vec2fixed<T> operator/(const vec2fixed<T> &other) const {
			return vec2fixed<T>(x / other.x,
								y / other.y);
		}

		vec2fixed<T> operator/(const T &other) const {
			return vec2fixed<T>(x / other,
								y / other);
		}

		void operator/=(const vec2fixed<T> &other) {
			x /= other.x;
			y /= other.y;
		}

		void operator/=(const T &other) {
			x /= other;
			y /= other;
		}

		vec2fixed<T> operator+(const vec2fixed<T> &other) const {
			return vec2fixed<T>(x + other.x,
								y + other.y);
		}

		vec2fixed<T> operator+(const T &other) const {
			return vec2fixed<T>(x + other,
								y + other);
		}

		void operator+=(const vec2fixed<T> &other) {
			x += other.x;
			y += other.y;
		}

		void operator+=(const T &other) {
			x += other;
			y += other;
		}

		vec2fixed<T> operator-(const vec2fixed<T> &other) const {
			return vec2fixed<T>(x - other.x,
								y - other.y);
		}

		vec2fixed<T> operator-(const T &other) const {
			return vec2fixed<T>(x - other,
								y - other);
		}

		void operator-=(const vec2fixed<T> &other) {
			x -= other.x;
			y -= other.y;
		}

		void operator-=(const T &other) {
			x -= other;
			y -= other;
		}

		bool operator==(const vec2fixed<T> &other) const {
			return x == other.x && y == other.y;
		}

		bool operator!=(const vec2fixed<T> &other) const {
			return x != other.x || y != other.y;
		}

	};

	template<typename T>
	struct vec3fixed {
		T x, y, z;


		vec3fixed(uint32_t x, uint32_t y, uint32_t z) : x(T(x)), y(T(y)), z(T(z)) {

		}

		vec3fixed(int32_t x, int32_t y, int32_t z) : x(T(x)), y(T(y)), z(T(z)) {

		}

		vec3fixed() : x(0), y(0), z(0) {

		}

		vec3fixed(T x, T y, T z) : x(x), y(y), z(z) {

		}

		static T dot(vec3fixed<T> &f1, vec3fixed<T> &f2) {
			return (f1.x * f2.x) + (f1.y * f2.y) + (f1.z * f2.z);
		}

		T dot(vec3fixed<T> &other) {
			return vec3fixed::dot(*this, other);
		}

		std::string toString() {
			return "(" + x.toString() + "," + y.toString() + "," + z.toString() + ")";
		}

		vec3fixed<T> normalized() {
			T mag = magnitude();
			return vec3fixed<T>(x / mag,
								y / mag,
								z / mag);
		}

		T pow2Magnitude() {
			return T((x * x) + (y * y) + (z * z));
		}

		T magnitude() {
			return sqrt((x * x) + (y * y) + (z * z));
		}

		void normalize() {
			T lenght = magnitude();

			x /= lenght;
			y /= lenght;
			z /= lenght;
		}


		glm::vec3 toVec3() {
			return {static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
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

		vec3fixed<T> operator*(const vec3fixed<T> &other) const {
			return vec3fixed<T>(x * other.x,
								y * other.y,
								z * other.z);
		}

		vec3fixed<T> operator*(const T &other) const {
			vec3fixed(x * other,
					  y * other,
					  z * other);
		}

		void operator*=(const vec3fixed<T> &other) {
			x *= other.x;
			y *= other.y;
			z *= other.z;
		}

		void operator*=(const T &other) {
			x *= other;
			y *= other;
			z *= other;
		}

		vec3fixed<T> operator/(const vec3fixed<T> &other) const {
			return vec3fixed<T>(x / other.x,
								y / other.y,
								z / other.z);
		}

		vec3fixed<T> operator/(const T &other) const {
			vec3fixed<T>(x / other,
						 y / other,
						 z / other);
		}

		void operator/=(const vec3fixed<T> &other) {
			x /= other.x;
			y /= other.y;
			z /= other.z;
		}

		void operator/=(const T &other) {
			x /= other;
			y /= other;
			z /= other;
		}

		vec3fixed<T> operator+(const vec3fixed<T> &other) const {
			return vec3fixed<T>(x + other.x,
								y + other.y,
								z + other.z);
		}

		vec3fixed<T> operator+(const T &other) const {
			vec3fixed<T>(x + other,
						 y + other,
						 z + other);
		}

		void operator+=(const vec3fixed<T> &other) {
			x += other.x;
			y += other.y;
			z += other.z;
		}

		void operator+=(const T &other) {
			x += other;
			y += other;
			z += other;
		}

		vec3fixed<T> operator-(const vec3fixed<T> &other) const {
			return vec3fixed<T>(x - other.x,
								y - other.y,
								z - other.z);
		}

		vec3fixed<T> operator-(const T &other) const {
			vec3fixed<T>(x - other,
						 y - other,
						 z - other);
		}

		void operator-=(const vec3fixed<T> &other) {
			x -= other.x;
			y -= other.y;
			z -= other.z;
		}

		void operator-=(const T &other) {
			x -= other;
			y -= other;
			z -= other;
		}

		bool operator==(const vec3fixed<T> &other) const {
			return x == other.x && y == other.y && z == other.z;
		}

		bool operator!=(const vec3fixed<T> &other) const {
			return x != other.x || y != other.y || z != other.z;
		}
	};

	typedef vec2fixed<fix16> vec2fix16;
	typedef vec3fixed<fix16> vec3fix16;
	typedef vec2fixed<fix8> vec2fix8;
	typedef vec2fixed<fix8> vec3fix8;
	typedef vec2fixed<fix24> vec2fix24;
	typedef vec2fixed<fix24> vec3fix24;

	template<typename T>
	vec2fixed<T> maxVec2Fix(vec2fixed<T> a, vec2fixed<T> b) {

		return vec2fixed<T>(std::max(a.x, b.x), std::max(a.y, b.y));
	}

	template<typename T>
	vec3fixed<T> maxVec2Fix(vec3fixed<T> a, vec3fixed<T> b) {
		return vec3fixed<T>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
	}

	template<typename T>
	vec2fixed<T> minVec3Fix(vec2fixed<T> a, vec2fixed<T> b) {
		return vec2fixed<T>(std::min(a.x, b.x), std::min(a.y, b.y));
	}

	template<typename T>
	vec3fixed<T> minVec3Fix(vec3fixed<T> a, vec3fixed<T> b) {
		return vec3fixed<T>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
	}

}
