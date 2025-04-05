#include <iostream>
#include "raylib.h"
#include "mewall.h"
#include "ConsoleRenderer.hpp"
#include "LoopWaiter.hpp"

#define Alive (byte)1
#define Dead (byte)0
#define IsAlive(cr) ((cr)==Alive)
#define IsDead(cr) ((cr)==Dead)

size_t cell_size = 20;

void DrawGrid(int width, int height, int cellSize) {
	for (int x = 0; x <= width; x += cellSize) {
		DrawLine(x, 0, x, height, BLACK);
	}
	for (int y = 0; y <= height; y += cellSize) {
		DrawLine(0, y, width, y, BLACK);
	}
}

void Printer(std::ostream& os, byte cur) {
	os << (IsAlive(cur)?'#': '.');
}

mew::game::ConsoleRenderer renderer(100U, 100U, Printer);

void RenderCells() {
	for (int x = 0; x < renderer.Width(); ++x) {
		for (int y = 0; y < renderer.Height(); ++y) {
			DrawRectangle(x*cell_size, y*cell_size, cell_size, cell_size, IsAlive(renderer.buffer.get(x, y))?BLACK:WHITE);
		}
	}
}

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
	// system("CLS");
	RenderCells();
	DrawGrid(renderer.Width()*cell_size, renderer.Height()*cell_size, cell_size);
	// renderer.Render(std::cout, '\n');
}

int GetRandomValue() {
	return GetRandomValue(0, INT_MAX);
}

void fillRandom() {
	for (int x = 0; x < renderer.Width(); ++x) {
		for (int y = 0; y < renderer.Height(); ++y) {
			renderer.buffer.set(x, y, (GetRandomValue()%2)?Alive:Dead);
		}
	}
	renderer.buffer.apply();
}

int main() {
	constexpr size_t __time = 100U;
	InitWindow(500, 500, "Game of Life");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(0);
	start();
	mew::game::LoopWaiter waiter;
	while(!WindowShouldClose()) {
		PollInputEvents();
		if (IsKeyPressed(KEY_ESCAPE)) {
			break;
		}
		if (IsKeyPressed(KEY_SPACE)) {
			renderer.buffer.clear(Dead);
			fillRandom();
		}
		cell_size += GetMouseWheelMove();
		if (waiter.Wait(__time)) {
			ClearBackground(RAYWHITE);
			BeginDrawing();
				render();
				update();
				DrawText("Press SPACE to fill random cells", 10, 10, 20, RED);
				DrawText("Press ESC to exit", 10, 30, 20, RED);
			EndDrawing();
		}
		// 	printf("time: %llu", __time);
		// }
	}
}

