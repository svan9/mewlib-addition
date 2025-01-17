#ifndef LOOP_WAITER_SO2U
#define LOOP_WAITER_SO2U

#include <ctime>
#include <ratio>
#include <chrono>
#include "mewall.h"

namespace mew::game {
	class LoopWaiter {
	private:
		std::chrono::system_clock::time_point _prev;
		std::chrono::system_clock::time_point _cur;
	public:
		LoopWaiter(): _prev(std::chrono::system_clock::now()) {}
		
		bool Wait(const size_t _milliseconds) {
			_cur = std::chrono::system_clock::now();
			if (
				std::chrono::duration_cast<std::chrono::milliseconds>
					(_cur - _prev).count() >= _milliseconds) 
			{
				_prev = _cur;
				return true;
			}
			return false;
		}
	};
}


#endif