#include <iostream>
#include "mewall.h"
#include "ConsoleRenderer.hpp"
#include "LoopWaiter.hpp"

#define Alive (byte)1
#define Dead (byte)0
#define IsAlive(cr) ((cr)==Alive)
#define IsDead(cr) ((cr)==Dead)

void Printer(std::ostream& os, byte cur) {
	os << (IsAlive(cur)?'#': '.');
}

mew::game::ConsoleRenderer renderer(20U, 10U, Printer);

void start() {
  renderer.buffer.clear(Dead);
	/*******************************/
  renderer.buffer.set(3, 1, Alive);
	/*******************************/
  renderer.buffer.set(1, 2, Alive);
  renderer.buffer.set(3, 2, Alive);
	/*******************************/
  renderer.buffer.set(2, 3, Alive);
  renderer.buffer.set(3, 3, Alive);
	/*******************************/
	renderer.buffer.apply();
}

void update() {
	renderer.buffer.sync();
	for (int x = 0; x < renderer.Width(); ++x) {
    for (int y = 0; y < renderer.Height(); ++y) { 
			size_t neightbors = renderer.buffer.calc_near(x, y, Alive);
			byte current = renderer.buffer.get(x, y);
			if (IsDead(current) && neightbors == 3) {
				renderer.buffer.set(x, y, Alive);
      }
      else if (IsAlive(current) && (neightbors < 2 || neightbors > 3)) {
				renderer.buffer.set(x, y, Dead);
      }
		} 
	}
	renderer.buffer.apply();
}

void render() {
	system("CLS");
	renderer.Render(std::cout, '\n');
}

int main() {
	constexpr size_t __time = 500U;
	start();
	mew::game::LoopWaiter waiter;
	while(true) {
		if (waiter.Wait(__time)) {
			render();
			update();
			printf("time: %llu", __time);
		}
	}
}

