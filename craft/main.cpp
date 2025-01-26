extern "C" {
	#include "raylib.h"
}
#include "mewall.h"
#include "world.hpp"
#include "data_set.hpp"
#include "utilities.hpp"


constexpr const uint _world_size = 256;
World world(_world_size, _world_size);

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
	ParticleSystem* floor_ps = GetParticleSystemFloor();
	DataSet* data_set = getDataSet();
	data_set->load();
	ParticleCluster put_block_ps(32);
	put_block_ps.produce((Color){ 0xcc, 0xcc, 0xcc, (byte)(0xFF*0.4f) }, 1.2f, 180.0f, 0.0f, 0.5f, 2.0f, 2.0f, 
		(Rectangle){16,16,0,0}, (Rectangle){-4,-4,40,40});
	ParticleCluster destroy_block_ps(32);
	destroy_block_ps.produce((Color){ 0x11, 0x11, 0x11, (byte)(0xFF*0.7f) }, 1.0f, 360.0f, 0.0f, 0.8f, 2.0f, 2.0f, 
		(Rectangle){-4,-4,40,40}, (Rectangle){16,16,8,8});
	floor_ps->add("put_block", put_block_ps);
	floor_ps->add("destroy_block", destroy_block_ps);
	storage->upload("resources/images/empty.png", "empty", "empty");
	storage->upload("resources/images/empty2.png", "empty2", "empty2");
	storage->upload("resources/images/player.png", "player", "player", CellInfoAnimation::Rows, 32, 32);
	storage->upload("resources/images/pipe.png", "pipe", "pipe", CellInfoAnimation::Rows, 32, 32);
	storage->upload("resources/images/sand1.png", "sand1", "sand1");
	storage->upload("resources/images/sand2.png", "sand2", "sand2");
	storage->upload("resources/images/sand3.png", "sand3", "sand3");
	storage->upload("resources/images/sand4.png", "sand4", "sand4");
	storage->upload("resources/images/sand5.png", "sand5", "sand5");
/* end upload textures */
	Player main_player("main_player", storage->getID("player"));
	main_player.setBlock(storage->getID("empty2"));
	world.createFloor(storage->getID("sand1"));
	world.PutForNoiseLayer(
		{
			storage->getID("sand1"), storage->getID("sand2"),
			storage->getID("sand3"), storage->getID("sand4"), 
			storage->getID("sand5")
		}, { 1.0, 0.01, 0.01, 0.01, 0.01 }
	);
	world.createLayer();
	world.StepLayerUp();
	// SetTargetFPS(144);
	size_t stored_w, stored_h;
	while (!WindowShouldClose()) {
		PollInputEvents();
		/* PRE UPDATE */
		WorldContext::Update(world, main_player);
		_e_key_f11(stored_w, stored_h);
		if (!IsWindowFullscreen()) {
			stored_w = GetScreenWidth();
			stored_h = GetScreenHeight();
		}
		main_player.zoomit();
		main_player.Update();
		WorldContext::Render(world);
		Vector2 v2 = WorldContext::GetCellPosByMouse(main_player.camera, world);
		/* END UPDATE */
		/* DRAWING */
		BeginDrawing();
			BeginMode2D(main_player.camera);
				ClearBackground(DARKGRAY);
				WorldContext::Draw(world);
				main_player.draw();
			EndMode2D();
			DrawText(TextFormat("fps: %i", GetFPS()), 5, 5, 20, WHITE);
			DrawText(TextFormat("zoom: %.2f", main_player.camera.zoom), 5, 25, 20, WHITE);
			DrawText(TextFormat("position: {x: %.0f; y: %.0f}",
				main_player.position.x, main_player.position.y), 5, 50, 20, WHITE);
			DrawText(TextFormat("mouse_cell: {x: %.0f; y: %.0f}",
				v2.x, v2.y), 5, 75, 20, WHITE);
			DrawText(TextFormat("frame time: %.5f", GetFrameTime()*1000), 5, 95, 20, WHITE);
			DrawText(main_player.body.toString(), 5, 115, 20, WHITE);
			if (current_data_set->states.show_slutch_message) {
				DrawText("WARN!! EMERGENCY BRAKING", GetScreenWidth(), GetScreenHeight(), 25, RED);
			}
			// {
			// 	static char buffer[256] = {0};
			// 	static bool text_focused = false;
			// 	InputText(buffer, 256, &text_focused, 5, 5, 20, 100, 50, WHITE, BLACK);
			// }
		EndDrawing();
		/* END DRAWING */
		/* POS UPDATE */
		_e_key_f11(stored_w, stored_h);
		WorldContext::Update(world, main_player);
		main_player.zoomit();
		main_player.Update();
		/* END POS UPDATE */
	}
	storage->clear();
	delete storage;
	world.clear();
	CloseWindow();
	return 0;
}