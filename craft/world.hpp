#ifndef WORLD_HPP
#define WORLD_HPP

#include "mewall.h"
#include "vector.hpp"
#include "noise.hpp"
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <initializer_list>
extern "C" {
	#include "raylib.h"
}

const float cell_size = 25;
const auto& same_s = mew::string::SameStr;

typedef uint CellID;
static CellID empty_cell = -1;

struct DynCellInfo { 
	const char* script_path;
};

struct CellInfoAnimation {
	enum Direction: byte {
		Rows, Colums, RowsWithColumns,
	};
	float x, y, w, h, frame_w, frame_h;
	Direction direction = Rows;
};

struct CellInfo {
	const char* name;
	const char* display_name;
	Image image;
	Texture2D texture;
	CellInfoAnimation* animation = nullptr;
	DynCellInfo* dyn_info = nullptr;
};

class GameStorage {
public:
	////////////////////////////////////////////////////////////
	std::vector<CellInfo> cells_info;

	////////////////////////////////////////////////////////////
	GameStorage() {
		CellInfo ci;
		ci.display_name = "error_block";
		ci.name 				= "error_block";
		cells_info.push_back(ci);
	}

	////////////////////////////////////////////////////////////
	GameStorage& add(CellInfo& info) {
		cells_info.push_back(info);
		return *this;
	}

	////////////////////////////////////////////////////////////
	CellID getID(const char* name) {
		for (CellID id = 0; id < cells_info.size(); ++id) {
			if (same_s(cells_info[id].name, name)) {
				return id;
			}
		}
		return empty_cell;
	}

	////////////////////////////////////////////////////////////
	CellInfo* get(const char* name) {
		for (CellID id = 0; id < cells_info.size(); ++id) {
			if (same_s(cells_info[id].name, name)) {
				return &cells_info[id];
			}
		}
		return nullptr;
	}

	////////////////////////////////////////////////////////////
	CellInfo* get(CellID id) {
		if (!id >= cells_info.size()) {
			return nullptr;
		}
		return &cells_info[id];
	}

	////////////////////////////////////////////////////////////
	bool is_dyn(const char* name) {
		CellInfo* info = get(name);
		MewUserAssert(info != nullptr, "cannot find block");
		return info->dyn_info != nullptr;
	}

	////////////////////////////////////////////////////////////
	bool is_dyn(CellID id) {
		CellInfo* info = get(id);
		MewUserAssert(info != nullptr, "cannot find block");
		return info->dyn_info != nullptr;
	}

	////////////////////////////////////////////////////////////
	CellInfo& upload(const char* path, const char* diplay_name, const char* name) {
		Image image = LoadImage(path);
		// ImageFlip(&image);
		Texture2D texture = LoadTextureFromImage(image);  
		UnloadImage(image);
		CellInfo cm_info;
		cm_info.display_name  = diplay_name;
		cm_info.name 				  = name;
		cm_info.image 	  		= image;
		cm_info.texture 		  = texture;
		cells_info.push_back(cm_info);
		return cells_info.back();
	}

	////////////////////////////////////////////////////////////
	CellInfo& uploadAnimated(
		const char* path, const char* diplay_name, const char* name, 
		CellInfoAnimation::Direction direction, size_t frame_w, size_t frame_h 
	){
		Image image = LoadImage(path);
		Texture2D texture = LoadTextureFromImage(image);  
		UnloadImage(image);
		CellInfo cm_info;
		cm_info.display_name  = diplay_name;
		cm_info.name 				  = name;
		cm_info.image 	  		= image;
		cm_info.texture 		  = texture;
		CellInfoAnimation* anima = new CellInfoAnimation();
		anima->x = 0;
		anima->y = 0;
		anima->w = image.width;
		anima->h = image.height;
		anima->frame_w = frame_w;
		anima->frame_h = frame_h;
		anima->direction = direction;
		cm_info.animation = anima;
		cells_info.push_back(cm_info);
		return cells_info.back();
	}

	////////////////////////////////////////////////////////////
	void free() {
		for (uint i = 0; i < cells_info.size(); ++i) {
			auto info = cells_info[i];
			UnloadTexture(info.texture);
		}
	}
};

static GameStorage* current_storage = nullptr;
GameStorage* GetCurrentGameStorage() {
	if (current_storage == nullptr) {
		current_storage = new GameStorage();
	}
	return current_storage;
}

typedef char* DynCellData;

#pragma pack(push, 1)
struct CellType {
	byte has_static: 1 = false;
	byte has_dynamic: 1 = false;
};
#pragma pack(pop)

class Layer {
private:
	std::vector<CellID> m_blocks;
	std::vector<DynCellData> m_dyn_blocks;
public:
	////////////////////////////////////////////////////////////
	Layer() {}
	
	////////////////////////////////////////////////////////////
	void fill(size_t size, CellID id = empty_cell) {
		m_blocks.resize(size, id);
		m_dyn_blocks.resize(size, 0);
	}

	////////////////////////////////////////////////////////////
	void set(size_t idx, CellID cell, DynCellData data = nullptr) {
		MewAssert(current_storage != nullptr);
		m_blocks[idx] = cell;
		if (current_storage->is_dyn(cell) && data != nullptr) {
			m_dyn_blocks[idx] = data;
		}
	}

	////////////////////////////////////////////////////////////
	CellID& get(size_t idx) {
		MewUserAssert(idx < m_blocks.size(), "undefined cell id");
		return m_blocks[idx];
	}

	////////////////////////////////////////////////////////////
	bool is_dyn(size_t idx) {
		MewAssert(current_storage != nullptr);
		CellID cid = get(idx);
		return GetCurrentGameStorage()->is_dyn(cid);
	}

	////////////////////////////////////////////////////////////
	bool is_static(size_t idx) {
		MewAssert(current_storage != nullptr);
		CellID cid = get(idx);
		return !GetCurrentGameStorage()->is_dyn(cid);
	}

	////////////////////////////////////////////////////////////
	CellType get_type(size_t idx) {
		CellType ct;
		ct.has_static  = is_static(idx);
		ct.has_dynamic = is_dyn(idx);
		return ct;
	}
};
class CellContext {
public:
	static void DrawAnimated(float x, float y, CellInfo* ci) {
		CellInfoAnimation* anima = ci->animation;
		Rectangle rect;
		rect.x = anima->x;
		rect.y = anima->y;
		rect.width = anima->frame_w;
		rect.height = anima->frame_h;
		Vector2 pos = {x, y};
		DrawTextureRec(ci->texture, rect, pos, WHITE);
		switch (anima->direction) {
			case CellInfoAnimation::Rows:
				anima->x += anima->frame_w;
				anima->x = mew::rclamp(anima->x, 0.0f, anima->w);
			break;
			case CellInfoAnimation::Colums:
				anima->y += anima->frame_h;
				anima->y = mew::rclamp(anima->y, 0.0f, anima->h);
			break;
			case CellInfoAnimation::RowsWithColumns:
				// todo check for work'
				anima->x += anima->frame_w;
				mew::rclamp2d(
					anima->x, anima->y, 
					0, anima->w, 
					0, anima->h, 
					anima->frame_w, anima->frame_h
				);
			break;
			default: MewUserAssert(false, "unsupported animation format"); break;
		}
	}
	static void Draw(float x, float y, CellInfo* ci) {
		if (ci->animation != nullptr) {
			DrawAnimated(x, y, ci);
		} else {
			DrawTexture(ci->texture, x, y, WHITE);
		}
	}
};

class World {
private:
	size_t width, height;
	std::vector<Layer> layers;
	uint current_layer = 0;
	bool should_render = true;
	RenderTexture2D main_texture;
public:
	World() {}
	World(size_t width, size_t height): width(width), height(height) {}

	////////////////////////////////////////////////////////////
	Rectangle getRect(){
		return {0,0, (float)width*cell_size, (float)height*cell_size};
	}

	////////////////////////////////////////////////////////////
	void createFloor(CellID fill) {
		MewUserAssert(fill != -1, "cannot puts empty cell");
		Layer _floor;
		_floor.fill(width*height, fill);
		layers.push_back(_floor);
		main_texture = LoadRenderTexture(width*cell_size, height*cell_size);
	}

	////////////////////////////////////////////////////////////
	void PutForNoiseLayer(std::initializer_list<CellID> cells, std::initializer_list<double> counts) {
		Layer& l =	getCurrentLayer();
		#pragma omp parallel for collapse(2)
		for (size_t i = 0; i < cells.size(); ++i) {
			size_t dens = width*height*((counts.begin())[i]);
			for (uint k = 0; k < dens; ++k) {
				size_t x = GetRandomValue(0, width);
				size_t y = GetRandomValue(0, height);
				l.set(mew::get_index(x, y, width), (cells.begin())[i]);
			}
		}
	}

	////////////////////////////////////////////////////////////
	RenderTexture2D& render() {
		if (!should_render) { return main_texture; }
		MewAssert(current_storage != nullptr);
		Layer& l = getCurrentLayer();
		BeginTextureMode(main_texture);
		#pragma omp parallel for collapse(2)
		for (uint x = 0; x < width; ++x) {
			for (uint y = 0; y < height; ++y) {
				size_t index = mew::get_index(x, y, width);
				// auto type = l.get_type(index);
				// MewUserAssert(!type.has_dynamic, "unsupported");
				CellID cid = l.get(index);
				CellInfo* block = current_storage->get(cid);
				CellContext::Draw(x*cell_size, y*cell_size, block);
				// DrawTexture(block->texture, x*cell_size, y*cell_size, WHITE);
			}
		}
		EndTextureMode();
		should_render = false;
		return main_texture;
	}
	
	////////////////////////////////////////////////////////////
	void set(size_t x, size_t y, CellID idx, DynCellData data = nullptr) {
		getCurrentLayer().set(mew::get_index(x, y, width), idx, data);
		should_render = true;
	}
	
	////////////////////////////////////////////////////////////
	Layer& getCurrentLayer() {
		return layers[current_layer];	
	}

	////////////////////////////////////////////////////////////
	vec2 bounds() {
		return vec2{(float)width, (float)height};
	}
	
};

class Player {
public:
	Vector2 position = {0,0};
	Camera2D camera;
	CellID player_cell;
	float lr_speed = 15.0f;
	float ud_speed = 15.0f;

	Player(CellID _player_cell) : player_cell(_player_cell) {
		camera.rotation = 180.0f;
		camera.zoom = 2.0f;
	}

	void zoomit() {
		camera.zoom += ((float)GetMouseWheelMove()*0.1f);
		camera.zoom = mew::clamp(camera.zoom, 0.1f, 3.0f);
	}

	void Update() {
		if (IsKeyDown(KEY_D)) {
			position.x -= lr_speed/cell_size;
		}
		if (IsKeyDown(KEY_A)) {
			position.x += lr_speed/cell_size;
		}
		if (IsKeyDown(KEY_W)) {
			position.y += ud_speed/cell_size;
		}
		if (IsKeyDown(KEY_S)) {
			position.y -= ud_speed/cell_size;
		}
		camera.target = position;
		Image img = current_storage->get(player_cell)->image;
		camera.offset = {
			-((img.width/camera.zoom)/2) + GetScreenWidth()/2.0f, 
			-((img.height/camera.zoom)/2) + GetScreenHeight()/2.0f};
	}

	void fox(World& w) {
		auto rect = w.getRect();
		position.x = mew::clamp(position.x, rect.x, rect.width);
		position.y = mew::clamp(position.y, rect.y, rect.height);
	}

	void draw() {
		MewAssert(current_storage != nullptr);
		CellInfo* block = current_storage->get(player_cell);
		MewUserAssert(block != nullptr, "cannot load player texture");
		CellContext::Draw(position.x, position.y, block);
	}
};

class WorldContext {
public:
	static void draw(World& w) {
		RenderTexture2D _texture = w.render();
		auto r = w.getRect();
		DrawTextureRec(_texture.texture, r, {-r.width/2, -r.height/2}, WHITE);
	}
};


#endif