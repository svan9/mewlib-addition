#ifndef DOUBLE_BUFFER_SO2U
#define DOUBLE_BUFFER_SO2U

#include "mewall.h"
#include <string.h>

namespace mew::game {
	template<typename T>
	class DoubleBuffer2d {
	private:
		size_t width, height;
		T* _main_buffer;
		T* _sub_buffer;
	public:
		////////////////////////////////////////////////////////////
		DoubleBuffer2d() {}

		////////////////////////////////////////////////////////////
		DoubleBuffer2d(size_t _w, size_t _h)
			: width(_w), height(_h), 
			_main_buffer(new T[size()]), _sub_buffer(new T[size()]) {
			}

		void clear(T val) {
			for (size_t i = 0; i < size(); ++i) {
				memcpy(_main_buffer+(i*sizeof(T)), &val, sizeof(T));
				memcpy(_sub_buffer+(i*sizeof(T)), &val, sizeof(T));
			}
		}

		////////////////////////////////////////////////////////////
		size_t size() const noexcept {
			return width*height;
		}

		////////////////////////////////////////////////////////////
		size_t Width() const noexcept {
			return width;
		}

		////////////////////////////////////////////////////////////
		size_t Height() const noexcept {
			return height;
		}

		////////////////////////////////////////////////////////////
		size_t get_idx(size_t x, size_t y) {
			return (((x%width)*width)+(y%height));
		}

		////////////////////////////////////////////////////////////
		void set(size_t idx, T val) {
			_sub_buffer[idx] = val;
			// memcpy(_sub_buffer+(idx*sizeof(T)), &val, sizeof(T));
		}

		////////////////////////////////////////////////////////////
		void set(size_t x, size_t y, T val) {
			set(get_idx(x, y), val);
		}

		////////////////////////////////////////////////////////////
		T get(size_t idx) {
			if (idx > size()) {
				return (T)0;
			}
			return *(_main_buffer+(idx*sizeof(T)));
		}

		////////////////////////////////////////////////////////////
		T get(size_t x, size_t y) {
			return get(get_idx(x, y));
		}

		////////////////////////////////////////////////////////////
		void sync() {
			memcpy(_sub_buffer, _main_buffer, size()*sizeof(T));
		}

		////////////////////////////////////////////////////////////
		void apply() {
			memcpy(_main_buffer, _sub_buffer, size()*sizeof(T));
		}

		////////////////////////////////////////////////////////////
		size_t calc_square(size_t min_x, size_t min_y, size_t max_x, size_t max_y, T val) {
			size_t counter = 0U;
			for (int _x = min_x; _x <= max_x; ++_x) {
				for (int _y = min_y; _y <= max_y; ++_y) {
					if (_x >= 0 && _y >= 0 && get(_x, _y) == val) { counter++; }
				}
			}
			return counter;
		}
		
		////////////////////////////////////////////////////////////
		size_t calc_near(size_t x, size_t y, T val) {
			size_t counter = calc_square(x-1, y-1, x+1, y+1, val);
			if (get(x, y) == val) {
				--counter;
			}
			return counter;
		}



	};
}

#endif