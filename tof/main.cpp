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

class Enemy {
public:
	Vector2 position;
	float max_health;
	float health;
	float money;
	size_t currentPathIndex;
	Enemy(): currentPathIndex(0) { }

	bool died() { return health <= 0; }
	
	void reuse() { 
		position = {0,0};
		health = max_health; 
	}
};

struct EnemyInfo {
	float health;
	float money;
};

class Player {
public:
	float health = 14;
	float money = 50;
	float tick_money = 1.0f;
	float killed_enemies = 0;
	mew::Timer timer;

	Player() { }
	
	void update() {
		timer.update();
		if (timer > 1) {
			timer.reset();
			money += tick_money;
		}
	}

} player_info;

class Tower {
public:
	const char* name1 = nullptr;
	const char* name2 = nullptr;
	const char* name3 = nullptr;
	byte level = 0;
	Vector2 position;
	float range = 2.0f;
	float damage = 1.0f;
	float bullet_speed = 0.2f;
	float fire_rate = 0.2f;
	mew::Timer timer;
	bool can_shoot = false;
	Tower() { }
	Tower(const char* name1, const char* name2, const char* name3)
		: name1(name1), name2(name2), name3(name3)  { }

	const char* getName() {
		switch(level) {
			case 0: return name1;
			case 1: return name2;
			case 2: return name3;
			default: return name3;
		}
	}
	
	void upgrade() {
		if (level < 2) {
			level++;
			damage *= 5.0f;
			fire_rate *= 0.4f;
			bullet_speed *= 3.0f;
		}
	}

	void update() {
		timer.update();
		if (timer > fire_rate) {
			can_shoot = true;
			timer.reset();
		}
	}
};

struct BulletInfo {
	Vector2 position;
	Enemy* target;
	float speed = 1.0f;
	float damage;
	bool died = false;
};

enum struct CellType {
	None,
	Grass,
	Track,
	Tower
};

class Level {
public:
	std::vector<Tower> towers;
	std::vector<Enemy> enemies;
	std::vector<Vector2> path;
	CellType select_type;
	std::vector<BulletInfo> bullets;
	size_t width, height;
	Vector2 selected_cell = {-1, -1};
	bool down_cell_is_path = false;
	Vector2 hover_cell;
	byte** _temp_map;
	Level(size_t width, size_t height): width(width), height(height) {
		_temp_map = new byte*[width];
		for (int i = 0; i < width; ++i) {
			_temp_map[i] = new byte[height];
		}
	}

	void spawnEnemy(float health, float money) {
		Enemy enemy;
		enemy.position = path[0];
		enemy.health = health;
		enemy.max_health = health;
		enemy.money = money;
		enemies.push_back(enemy);
	}

	void spawnEnemy(EnemyInfo info) {
		Enemy enemy;
		enemy.position = path[0];
		enemy.health = info.health;
		enemy.max_health = info.health;
		enemy.money = info.money;
		enemies.push_back(enemy);
	}

	void generatePath(uint count) {
		path.clear();
		float step_y = height / count;
		if (count % 2 != 0) count++;
		for (size_t i = 0; i < count; ++i) {
			float x = (i % 2) * (width-2);
			if (x == 0) x = 1;
			float y = i * step_y - 1;
			if (y == 0) y = 1;
			path.push_back((Vector2){x, y});
			path.push_back((Vector2){x, y+(float)(int)step_y});
		}
		// if (path.back() != (Vector2){width-1.0f, height-1.0f}) {
		// 	path.push_back((Vector2){width-1.0f, height-1.0f});
		// }
	}

	void updateBullets() {
		for(auto& bullet: bullets) {
			Vector2 direction = Vector2Normalize(bullet.target->position - bullet.position);
			bullet.position = bullet.position + direction * bullet.speed;
			if (GetDistance(bullet.position, bullet.target->position) < 1.0f) {
				bullet.target->health -= bullet.damage;
				bullet.died = true;
			}
		}

		for (int i = bullets.size() - 1; i >= 0; --i) {
			if (bullets[i].died) {
				bullets.erase(bullets.begin() + i);
			}
		}
	}

	void updateEnemy(float move_speed) {
		// select_type = CellType::None;
		for (auto& enemy : enemies) {
			if (enemy.died()) {
				continue;
			}
			if (enemy.currentPathIndex < path.size() - 1) {
				Vector2 target = path[enemy.currentPathIndex + 1];
				Vector2 direction = target - enemy.position;
				float distance = Vector2Length(direction);
				if (distance < move_speed) {
					enemy.position = target;
					enemy.currentPathIndex++;
				} else {
					direction = Vector2Normalize(direction) * move_speed;
					enemy.position = enemy.position + direction;
				}
			}
		}
		for (int i = enemies.size() - 1; i >= 0; --i) {
			if (enemies[i].died()) {
				enemies.erase(enemies.begin() + i);
			}
		}

		for (auto& tower: towers) {
			tower.update();	
			Enemy* nearest = getNearestEnemy(tower);
			if (nearest != nullptr && tower.can_shoot) {
				BulletInfo bullet;
				bullet.damage = tower.damage;
				bullet.position = tower.position;
				bullet.target = nearest;
				bullet.speed = tower.bullet_speed;
				tower.can_shoot = false;
				bullets.push_back(bullet);
			}
		}
		updateBullets();
	}

	Enemy* getNearestEnemy(Tower& tower) {
		Enemy* nearest_enemy = nullptr;
		float min_distance = tower.range;
		for (auto& enemy : enemies) {
			if (enemy.died()) continue;
			float distance = GetDistance(tower.position, enemy.position);
			if (distance < min_distance) {
				min_distance = distance;
				nearest_enemy = &enemy;
			}
		}
		return nearest_enemy;
	}

	void draw() {
		float width_factor = width / height;
		// SetWindowSize(width_factor*width*cell_size, height*cell_size);
		width_factor = GetScreenWidth() / GetScreenHeight();
		float real_cell_size = cell_size;

		TextureAtlas* storage = GetCurrentTextureAtlas();
		for (int x = 0; x < width; ++x) {
			for (int y = 0; y < height; ++y) {
				_temp_map[x][y] = 0;
			}
		}
		// grass->texture.width = real_cell_size;
		// grass->texture.height = real_cell_size;
		// track->texture.width = real_cell_size;
		// track->texture.height = real_cell_size;
		for (int x = 0; x < width; ++x) {
			for (int y = 0; y < height; ++y) {
				Vector2 position = (Vector2){x*real_cell_size, y*real_cell_size};
				storage->draw("grass", position);
			}
		}

		for (size_t i = 0; i < path.size() - 1; ++i) {
			Vector2 start = path[i];
			Vector2 end = path[i + 1];
			Vector2 direction = Vector2Subtract(end, start);
			float length = Vector2Length(direction);
			direction = Vector2Normalize(direction);

			for (float j = 0; j < length; ++j) {
				Vector2 position = Vector2Add(start, Vector2Scale(direction, j));
				if (!(position.x < 0 || position.y < 0 || position.x >= width || position.y >= height)) {
					// position.x *= real_cell_size;
					// position.y *= real_cell_size;
					_temp_map[(int)position.x][(int)position.y] = 1;
					// storage->draw("track", position);
				}
			}
		}

		for (int x = 0; x < width; ++x) {
			for (int y = 0; y < height; ++y) {
				byte current = _temp_map[x][y];
				mew::RoadType rt = mew::getRoadType(_temp_map, width, height, x, y);
				const char* prefix = mew::rt_to_prefix(rt);
				char* name = mew::string::join_str(1 == current? "track": "grass", prefix, "-");
				storage->draw(name, (Vector2){x*real_cell_size, y*real_cell_size});
			}
		}

		for (auto& tower: towers) {
			if (tower.getName() != nullptr){
				storage->draw(tower.getName(), tower.position*real_cell_size);
			} else {
				DrawRectangle(tower.position.x*real_cell_size, tower.position.y*real_cell_size, 
					real_cell_size, real_cell_size, RED);
			}
		}
		
		for (auto& enemy : enemies) {
			if (enemy.health <= 0) {
				player_info.money += enemy.money;
				player_info.killed_enemies++;
				continue;
			}
			if (enemy.position == path.back()) {
				player_info.health -= 1;
				enemy.position.x = 0;
				enemy.position.y = 0;
				continue;
			}
			Vector2 position = enemy.position*real_cell_size;
			DrawRectangleV(position, (Vector2){real_cell_size, real_cell_size}, RED);
			DrawRectangle(position.x, position.y+real_cell_size-4, real_cell_size, 4, GRAY);
			DrawRectangle(position.x, position.y+real_cell_size-4, 
				(real_cell_size)*(enemy.health/enemy.max_health), 4, GREEN);
		}
		
		for (auto& bullet: bullets) {
			DrawCircle(bullet.position.x*real_cell_size, bullet.position.y*real_cell_size, 4, RED);
			// DrawRectangle(bullet.position.x*real_cell_size, bullet.position.y*real_cell_size, 
				// real_cell_size, real_cell_size, BLUE);
		}

		if (selected_cell != (Vector2){-1, -1}) {
			DrawRectangleLinesEx(
				(Rectangle){selected_cell.x*real_cell_size, selected_cell.y*real_cell_size, 
					real_cell_size, real_cell_size}, 2, ORANGE);
			DrawRectangleRec(
				(Rectangle){selected_cell.x*real_cell_size, selected_cell.y*real_cell_size, 
					real_cell_size, real_cell_size}, ColorAlpha(ORANGE, 0.5f));
		} 
		if (!(hover_cell.x < 0 || hover_cell.y < 0 ||
			hover_cell.x >= width || hover_cell.y >= height)) {
			if (down_cell_is_path) {
				DrawRectangleLinesEx(
					(Rectangle){hover_cell.x*real_cell_size, hover_cell.y*real_cell_size, 
						real_cell_size, real_cell_size}, 2, RED);
			} else {
				DrawRectangleLinesEx(
					(Rectangle){hover_cell.x*cell_size, hover_cell.y*cell_size, 
						cell_size, cell_size}, 2, GREEN);
			}

			for (auto& tower: towers) {
				if (tower.position == selected_cell) {
					select_type = CellType::Tower;
					DrawCircleGradient(
						tower.position.x*real_cell_size+(real_cell_size/2), 
						tower.position.y*real_cell_size+(real_cell_size/2), 
						(tower.range*real_cell_size), 
						(tower.range*real_cell_size) - 5,
						ColorAlpha(GREEN, 0.0f), GREEN);
				}
				if (tower.position == hover_cell) {
					DrawCircleGradient(
						tower.position.x*real_cell_size+(real_cell_size/2), 
						tower.position.y*real_cell_size+(real_cell_size/2), 
						(tower.range*real_cell_size),
						(tower.range*real_cell_size) - 5,
						ColorAlpha(GREEN, 0.0f), GREEN);
				}
			}
		}
	}

	void KeyUpdate() {
		Vector2 mcel = GetCellPosUnderMouse(cell_size);
		if (mcel.x < 0 || mcel.y < 0 || // skip out of area
			mcel.x >= width || mcel.y >= height) { return; }
		if (PointOnLineSegments(mcel, path)) { // on the road
			down_cell_is_path = true;
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				select_type = CellType::Track;
				selected_cell = {-1, -1};
			}
		} else { // at the street
			down_cell_is_path = false;
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				select_type = CellType::Grass;
				if (selected_cell == mcel) { // deselect
					selected_cell = {-1, -1};
				} else { // select
					selected_cell = mcel;
				}
			} 
		}
		hover_cell = mcel;
	}

	void putTower(Tower tower) {
		towers.push_back(tower);
	}

	void putTowerAtSelected(Tower tower) {
		tower.position = selected_cell;
		towers.push_back(tower);
	}

	void upgrade_tower() {
		for (auto& tower: towers) {
			if (tower.position == selected_cell) {
				tower.upgrade();
			}
		}
	}
	void add_range() {
		for (auto& tower: towers) {
			if (tower.position == selected_cell) {
				tower.range *= 1.5f;
			}
		}
	}
};

EnemyInfo skeleton = (EnemyInfo){20.0f, 50.0f};

class Button: public UI {
public:
	Button() { UI(); }
};

Button& GetButton(const char* text) {
	return (Button&)GetUICluster()->get(text);
}
UI& GetUI(const char* text) {
	return GetUICluster()->get(text);
}

void HideUI(const char* name) {
	GetUICluster()->hide(name);
}
void ShowUI(const char* name) {
	GetUICluster()->show(name);
}

Level* current_level = new Level(20, 10);
Level* GetLevelCurrent() {
	return current_level;
}

float tower1_price = 20;
float upgrade_price = 50;
int step_speed = 7;
float move_speed = 0.05f;

void DrawAddTower(Vector2 pos, Vector2 size, Color tint) {
	TextureAtlas* storage = GetCurrentTextureAtlas();
	storage->draw("add_tower", pos, size, tint);
}
void DrawAddRange(Vector2 pos, Vector2 size, Color tint) {
	TextureAtlas* storage = GetCurrentTextureAtlas();
	storage->draw("add_range", pos, size, tint);
}
void DrawUpgradeTower(Vector2 pos, Vector2 size, Color tint) {
	TextureAtlas* storage = GetCurrentTextureAtlas();
	storage->draw("upgrade_tower", pos, size, tint);
}

int main(void) {
	InitWindow(800, 450, "tower of defense");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	
	TextureAtlas* storage = GetCurrentTextureAtlas();
	storage->loadAtlas("resources/tof/tilemap.png", 32, 32);

	storage->upload(1, 			"track-single");
	storage->upload(2, 			"grass-single");


	storage->upload(/*6, */2, 		"grass-top");
	storage->upload(/*8+4, */2,		"grass-left");
	storage->upload(/*8+8+5,*/2,  "grass-bottom");
	storage->upload(/*8+8+6,*/2, 	"grass-right_top_corner");
	storage->upload(/*8+8+6,*/2, 	"grass-left_bottom_corner", 	0,1);
	storage->upload(/*8+8+6,*/2, 	"grass-left_top_corner", 			1,0);
	storage->upload(/*8+8+6,*/2, 	"grass-right_bottom_corner", 	1,1);
	storage->upload(/*8+8, */2,		"grass-right");


	storage->upload(13,  		"track-veritcal");
	storage->upload(14, 		"track-horizontal");
	storage->upload(23, 		"track-right_top_corner");
	storage->upload(23, 		"track-right_bottom_corner",	0,1);
	storage->upload(23, 		"track-left_top_corner", 		  1,0);
	storage->upload(23, 		"track-left_bottom_corner", 	1,1);
	
	storage->upload(3, 			"add_range");
	storage->upload(4, 			"add_tower");
	storage->upload(5, 			"upgrade_tower");
	storage->upload(9, 			"tower1");
	storage->upload(10, 		"tower2");
	storage->upload(11, 		"tower3");
	
	GetLevelCurrent()->generatePath(3);
	UI& tower = GetUI("tower");
	tower.style.font_size = 20;
	tower.style.background = Background::fromAtlas(DrawAddTower);
	tower.style.box_align = Alignment::TopRight;
	tower.style.size = SemiVec::fromValue((Vector2){64.0f, 64.0f});
	tower.onHover = [](UI& ui) {
		SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
	};
	tower.onClick = [](UI& ui) {
		if (player_info.money - tower1_price < 0) { return; }
		if (GetLevelCurrent()->selected_cell != (Vector2){-1, -1}) {
			GetLevelCurrent()->putTowerAtSelected(Tower("tower1", "tower2", "tower3"));
			player_info.money -= tower1_price;
		}
	};

	UI& add_range = GetUI("add_range");
	add_range.style.font_size = 20;
	add_range.style.margin.top = 10;
	add_range.style.background = Background::fromAtlas(DrawAddRange);
	add_range.style.box_align = Alignment::TopRight;
	add_range.style.position = SemiVec::fromValue((Vector2){0.0f, 64.0f});
	add_range.style.size = SemiVec::fromValue((Vector2){64.0f, 64.0f});
	add_range.onHover = [](UI& ui) {
		SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
	};
	add_range.onClick = [](UI& ui) {
		if (player_info.money - upgrade_price < 0) { return; }
		if (GetLevelCurrent()->selected_cell != (Vector2){-1, -1}) {
			GetLevelCurrent()->add_range();
			player_info.money -= upgrade_price;
		}
	};
	UI& upgrade_tower = GetUI("upgrade_tower");
	upgrade_tower.style.font_size = 20;
	upgrade_tower.style.margin.top = 10*2;
	upgrade_tower.style.background = Background::fromAtlas(DrawUpgradeTower);
	upgrade_tower.style.box_align = Alignment::TopRight;
	upgrade_tower.style.position = SemiVec::fromValue((Vector2){0.0f, 64.0f*2});
	upgrade_tower.style.size = SemiVec::fromValue((Vector2){64.0f, 64.0f});
	upgrade_tower.onHover = [](UI& ui) {
		SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
	};
	upgrade_tower.onClick = [](UI& ui) {
		if (GetLevelCurrent()->selected_cell != (Vector2){-1, -1}) {
			GetLevelCurrent()->upgrade_tower();
		}
	};
	UI& end_menu = GetUI("end_menu");
	end_menu.style.background = Background::fromColor(ColorAlpha(BLACK, 0.8f));
	end_menu.style.size = SemiVec::fromPercent((Vector2){1.0f, 1.0f});

	UI& info = GetUI("info");
	info.style.box_align = Alignment::TopRight;
	info.style.size = SemiVec::fromPercent((Vector2){0.3f, 1.0f});
	info.style.background = Background::fromColor(ColorAlpha(BLACK, 0.5f));
	ShowUI("tower");
	ShowUI("info");
	ShowUI("add_range");
	ShowUI("upgrade_tower");

	RenderTexture2D field = LoadRenderTexture(
		GetLevelCurrent()->width*cell_size, GetLevelCurrent()->height*cell_size);

	double last = mew::getTimeSeconds();
	mew::Timer skeleton_timer;
	mew::Timer upgrade_timer;
	
	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		if (upgrade_timer >= 5) {
			step_speed *= 0.9f;
			skeleton.health *= 1.4f;
			upgrade_timer.reset();
		}
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
		GetLevelCurrent()->KeyUpdate();
		tower.style.disabled     = GetLevelCurrent()->select_type != CellType::Grass;
		add_range.style.disabled = GetLevelCurrent()->select_type != CellType::Tower;
		add_range.style.disabled = GetLevelCurrent()->select_type != CellType::Tower;
		GetUICluster()->update();
		skeleton_timer.update();
		player_info.update();
		GetLevelCurrent()->updateEnemy(move_speed);
		if (skeleton_timer >= step_speed) {
			GetLevelCurrent()->spawnEnemy(skeleton);
			skeleton_timer.reset();
		}
		PollInputEvents();
		BeginDrawing();
			ClearBackground(GRAY);
			GetLevelCurrent()->draw();
			GetUICluster()->render();
			DrawFPS(5, 5);
			DrawText(TextFormat(
				"skeleton delta: %is\n"
				"health: %i \n"
				"money: %i \n",
				(int)skeleton_timer.GetDeltaTime(),
				(int)player_info.health,
				(int)player_info.money
			), 5, 30, 20, LIME);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}