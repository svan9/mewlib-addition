#ifndef CONSOLE_RENDER_SO2U
#define CONSOLE_RENDER_SO2U

#include <iostream>
#include "mewall.h"
#include "DoubleBuffer.hpp"

namespace mew::game {
	void DefaultPrinter(std::ostream& os, byte current) {
		os << current;
	}

	class ConsoleRenderer {
	public:
		typedef void(*printer_t)(std::ostream&, byte);
		DoubleBuffer2d<byte> buffer;

		printer_t printer = DefaultPrinter;

		ConsoleRenderer() {}
		ConsoleRenderer(size_t w, size_t h): buffer(w, h) {}
		ConsoleRenderer(size_t w, size_t h, printer_t _printer): buffer(w, h), printer(_printer) {}

		void Render(std::ostream &out, const char splitter = '\n') {
			for (int y = 0; y < buffer.Height(); ++y) { 
				for (int x = 0; x < buffer.Width(); ++x) {
					printer(out, buffer.get(x, y));
				}
				out << splitter;
			}
		}

		size_t Width() const noexcept {
			return buffer.Width();
		}
		
		size_t Height() const noexcept {
			return buffer.Height();
		}
	};
}


#endif