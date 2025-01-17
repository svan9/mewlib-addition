extern "C" {
	#include "raylib.h"
}
#include "mewall.h"
#include "world.hpp"

constexpr const uint _world_size = 256;
World cw(_world_size, _world_size);

void _e_key_f11(size_t w, size_t h) {
	if(IsKeyPressed(KEY_F11)) {
		if (!IsWindowFullscreen()) {
			SetWindowSize(GetMonitorWidth(GetCurrentMonitor()), 
				GetMonitorHeight(GetCurrentMonitor()));
			ToggleFullscreen();
		} else {
			ToggleFullscreen();
			SetWindowSize(w, h);
		}
	}
}

int main(void) {
	InitWindow(800, 450, "craft");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
/* upload textures */
	GameStorage* storage = GetCurrentGameStorage();
	storage->upload("resources/images/empty.png", "empty", "empty");
	storage->uploadAnimated("resources/images/player.png", "player", "player", CellInfoAnimation::Rows, 32, 32);
	storage->upload("resources/images/sand1.png", "sand1", "sand1");
	storage->upload("resources/images/sand2.png", "sand2", "sand2");
	storage->upload("resources/images/sand3.png", "sand3", "sand3");
	storage->upload("resources/images/sand4.png", "sand4", "sand4");
	storage->upload("resources/images/sand5.png", "sand5", "sand5");
/* end upload textures */
	Player main_player(storage->getID("player"));
	cw.createFloor(storage->getID("sand1"));
	cw.PutForNoiseLayer(
		{
			storage->getID("sand1"), storage->getID("sand2"),
			storage->getID("sand3"), storage->getID("sand4"), 
			storage->getID("sand5")
		}, { 1.0, 0.01, 0.01, 0.01, 0.01 }
	);
	size_t stored_w, stored_h;
	while (!WindowShouldClose()) {
		PollInputEvents();
		_e_key_f11(stored_w, stored_h);
		if (!IsWindowFullscreen()) {
			stored_w = GetScreenWidth();
			stored_h = GetScreenHeight();
		}
		main_player.zoomit();
		main_player.Update();
		BeginDrawing();
			BeginMode2D(main_player.camera);
				ClearBackground(DARKGRAY);
				WorldContext::draw(cw);
				main_player.draw();
			EndMode2D();
			DrawText(TextFormat("fps: %i", GetFPS()), 5, 5, 20, BLACK);
			DrawText(TextFormat("zoom: %f", main_player.camera.zoom), 5, 25, 20, BLACK);
			DrawText(TextFormat("position: {x: %.2f; y: %.2f}",
				main_player.position.x, main_player.position.y), 5, 50, 20, BLACK);
			DrawText(TextFormat("window: {w: %i; h: %i}",
				GetScreenWidth(), GetScreenHeight()), 5, 75, 20, BLACK);
		EndDrawing();
		_e_key_f11(stored_w, stored_h);
	}
	storage->free();
	CloseWindow();
	return 0;
}