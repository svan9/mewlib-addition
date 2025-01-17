#ifndef VECTOR_SO2U
#define VECTOR_SO2U

#include "mewall.h"

namespace mew::game {
	template<typename T, size_t size>
	class Vector;

	template<typename T, size_t size>
	class Vector<T, 2> {
	public:
		typedef Vector<T, 2> self_type;
		union {
			struct { T x, y; };
		};
		Vector<T, 2>() { }
		Vector<T, 2>(T _x, T _y), x(_x), y(_y) { }

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

	typedef Vector<float, 2> vec2;
	typedef Vector<uint,  2> vec2u;
}

#endif