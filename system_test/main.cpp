extern "C" {
	#include "raylib.h"
}
#include "raymath.h"
#include "mewall.h"
#include "storage.hpp"
#include "physics.hpp"
#include "utilities.hpp"
#include "geomentry.hpp"
#include "ui.hpp"
#include "particles.hpp"

static const float cell_size = 32.0f;

int main(void) {
	InitWindow(800, 450, "tower of defense");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	
	TextureAtlas* storage = GetCurrentTextureAtlas();
	
	auto* ps = getParticleSystem();
	
	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
			ps->spawn(100, GetMousePosition(), (Vector2){3, 3}, 4000, (Vector2){0, 1000}, BLACK);
		}
		ps->update();
		PollInputEvents();
		BeginDrawing();
			ClearBackground(GRAY);
			ps->draw();
			DrawFPS(5, 5);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}