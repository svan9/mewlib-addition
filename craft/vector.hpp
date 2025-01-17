#ifndef VECTOR_SO2U
#define VECTOR_SO2U

#include "mewall.h"

namespace mew::game {
	template<uint L, typename T> struct vec;

	template<typename T>
	struct vec<2, T> {
		typedef vec<2, T> self_type;
		union {
			struct { T x, y; };
		};

		self_type& operator+=(const self_type& l) {
			x += l.x; 
			y += l.y;
			return *this;
		}
		self_type& operator-=(const self_type& l) {
			x -= l.x; 
			y -= l.y;
			return *this;
		}
		self_type& operator*=(const self_type& l) {
			x *= l.x; 
			y *= l.y;
			return *this;
		}
		self_type& operator/=(const self_type& l) {
			x /= l.x; 
			y /= l.y;
			return *this;
		}
		self_type& operator+=(T l) {
			x += l; 
			y += l;
			return *this;
		}
		self_type& operator-=(T l) {
			x -= l; 
			y -= l;
			return *this;
		}
		self_type& operator*=(T l) {
			x *= l; 
			y *= l;
			return *this;
		}
		self_type& operator/=(T l) {
			x /= l; 
			y /= l;
			return *this;
		}

		friend self_type operator+(const self_type& r, const self_type& l) {
			return self_type(r.x+l.x, r.y+l.y);
		}
		friend self_type operator-(const self_type& r, const self_type& l) {
			return self_type(r.x-l.x, r.y-l.y);
		}
		friend self_type operator*(const self_type& r, const self_type& l) {
			return self_type(r.x*l.x, r.y*l.y);
		}
		friend self_type operator/(const self_type& r, const self_type& l) {
			return self_type(r.x/l.x, r.y/l.y);
		}
		friend self_type operator+(const self_type& r, T l) {
			return self_type(r.x+l, r.y+l);
		}
		friend self_type operator-(const self_type& r, T l) {
			return self_type(r.x-l, r.y-l);
		}
		friend self_type operator*(const self_type& r, T l) {
			return self_type(r.x*l, r.y*l);
		}
		friend self_type operator/(const self_type& r, T l) {
			return self_type(r.x/l, r.y/l);
		}
	};

	typedef vec<2, float> vec2;
	typedef vec<2, uint> vec2u;
}
typedef mew::game::vec<2, float> vec2;
typedef mew::game::vec<2, uint> vec2u;

#endif