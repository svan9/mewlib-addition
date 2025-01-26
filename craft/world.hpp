#ifndef WORLD_HPP
#define WORLD_HPP

#include "mewall.h"
#include "noise.hpp"
#include "particles.hpp"
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <initializer_list>
#include "data_set.hpp"
#include "ui.hpp"
#include "inventory.hpp"

using mew::vec2;

extern "C" {
	#include "raylib.h"
}

const float cell_size = 32;
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
	float factor = 0.0f;
	float speed = 1.0f;
	Direction direction = Rows;
};

struct CellInfo {
	const char* name;
	const char* display_name;
	Image image;
	Texture2D texture;
	Color color;
	float rotation = 0.0f;
	CellInfoAnimation* animation = nullptr;
	DynCellInfo* dyn_info = nullptr;
};

// Function to get the mixed color from an image
Color GetMixedColorFromImage(Image image, Rectangle area) {
	Color mixedColor = {0, 0, 0, 255};
	int pixelCount = 0;

	for (int y = area.y; y < area.y + area.height; y++) {
		for (int x = area.x; x < area.x + area.width; x++) {
			Color pixelColor = GetImageColor(image, x, y);
			mixedColor.r += pixelColor.r;
			mixedColor.g += pixelColor.g;
			mixedColor.b += pixelColor.b;
			pixelCount++;
		}
	}

	if (pixelCount > 0) {
		mixedColor.r /= pixelCount;
		mixedColor.g /= pixelCount;
		mixedColor.b /= pixelCount;
	}

	return mixedColor;
}

// Function to get the mixed color from an image
Color GetMixedColorFromImage(Image image) {
	return GetMixedColorFromImage(image, (Rectangle){0,0, (float)image.width, (float)image.height});
}

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
	~GameStorage() {
		clear();
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
		CellInfo cm_info;
		cm_info.display_name  = diplay_name;
		cm_info.name 				  = name;
		cm_info.image 	  		= image;
		cm_info.color 				= GetMixedColorFromImage(image);
		cm_info.texture 		  = texture;
		cells_info.push_back(cm_info);
		UnloadImage(image);
		return cells_info.back();
	}

	////////////////////////////////////////////////////////////
	CellInfo& upload(
		const char* path, const char* diplay_name, const char* name, 
		CellInfoAnimation::Direction direction, size_t frame_w, size_t frame_h, 
		float speed = 1.0f
	){
		Image image = LoadImage(path);
		Texture2D texture = LoadTextureFromImage(image);  
		CellInfo cm_info;
		cm_info.display_name  = diplay_name;
		cm_info.name 				  = name;
		cm_info.image 	  		= image;
		cm_info.color 				= GetMixedColorFromImage(image);
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
		UnloadImage(image);
		cells_info.push_back(cm_info);
		return cells_info.back();
	}

	////////////////////////////////////////////////////////////
	void clear() {
		for (uint i = 0; i < cells_info.size(); ++i) {
			auto info = cells_info[i];
			UnloadTexture(info.texture);
		}
		cells_info.clear();
	}
};

static GameStorage* current_storage = nullptr;
GameStorage* GetCurrentGameStorage() {
	if (current_storage == nullptr) {
		current_storage = new GameStorage();
	}
	return current_storage;
}
static ParticleSystem* floor_particle_system = nullptr;
ParticleSystem* GetParticleSystemFloor() {
	if (floor_particle_system == nullptr) {
		floor_particle_system = new ParticleSystem();
	}
	return floor_particle_system;
}
static ParticleSystem* top_particle_system = nullptr;
ParticleSystem* GetParticleSystemTop() {
	if (top_particle_system == nullptr) {
		top_particle_system = new ParticleSystem();
	}
	return top_particle_system;
}

typedef char* DynCellData;

#pragma pack(push, 1)
struct CellType {
	byte has_static: 1 = false;
	byte has_dynamic: 1 = false;
};
#pragma pack(pop)

inline char* NaD = (char*)"NaD"; 

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
		m_dyn_blocks.resize(size, NaD);
	}

	////////////////////////////////////////////////////////////
	void set(size_t idx, CellID cell, DynCellData data = nullptr) {
		MewAssert(current_storage != nullptr);
		m_blocks[idx] = cell;
		if (cell != empty_cell && 
			current_storage->is_dyn(cell) && 
			data != nullptr) {
			m_dyn_blocks[idx] = data;
		}
	}

	////////////////////////////////////////////////////////////
	CellID& get(size_t idx) {
		MewUserAssert(idx < m_blocks.size(), "undefined cell id");
		return m_blocks[idx];
	}
	////////////////////////////////////////////////////////////
	DynCellData& get_dyn(size_t idx) {
		MewUserAssert(idx < m_dyn_blocks.size(), "undefined cell id");
		return m_dyn_blocks[idx];
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

Rectangle operator+(Rectangle& rect, Vector2 vec) {
	Rectangle __rect(rect);
	__rect.x += vec.x;
	__rect.y += vec.y;
	__rect.width += vec.x;
	__rect.height += vec.y;
	return __rect;
}

void DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, float rotation, Color tint) {
	Rectangle dest = { position.x, position.y, fabsf(source.width), fabsf(source.height) };
	Vector2 origin = { source.width/2.0f, source.height/2.0f };
	DrawTexturePro(texture, source, dest, origin, rotation, tint);
}
void DrawTexture(Texture2D texture, int posX, int posY, float rotation, Color tint) {
	DrawTextureEx(texture, (Vector2){ (float)posX, (float)posY }, rotation, 1.0f, tint);
}

class CellContext {
public:
	static void DrawAnimated(float x, float y, CellInfo* ci, DynCellData data = nullptr) {
		CellInfoAnimation* anima = ci->animation;
		Rectangle rect;
		rect.x = anima->x;
		rect.y = anima->y;
		rect.width = anima->frame_w;
		rect.height = anima->frame_h;
		Vector2 pos = {x, y};
		DrawTextureRec(ci->texture, rect, pos, ci->rotation, WHITE);
		if (++anima->factor >= anima->speed) {
			anima->factor = 0;
		} else { return; }

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

	static void Draw(float x, float y, CellInfo* ci, DynCellData data = nullptr) {
		if (ci->animation != nullptr) {
			DrawAnimated(x, y, ci, data);
		} else {
			DrawTexture(ci->texture, x, y, ci->rotation, WHITE);
		}
	}

	static bool inRectangle(Rectangle& rect, Vector2& v) {
		return (
			v.x > rect.x     &&
			v.x < rect.width &&
			v.y > rect.y     &&
			v.y < rect.height
		);
	}
};

class World {
friend class WorldContext;
public:
	typedef struct {
		RenderTexture2D main;
		RenderTexture2D sub;
	} RenderTextures;
private:
	size_t width, height;
	std::vector<Layer> layers;
	uint current_layer = 0;
	bool should_render = true;
	RenderTextures r_texture;
public:	
	World() {}
	World(size_t width, size_t height): width(width), height(height) {}

	////////////////////////////////////////////////////////////
	Rectangle getRect() {
		return (Rectangle){0,0, (float)width*cell_size, (float)height*cell_size};
	}

	////////////////////////////////////////////////////////////
	Vector2 getPos() {
		return (Vector2){(float)(width*cell_size)/-2.0f, (float)(height*cell_size)/-2.0f};
	}

	////////////////////////////////////////////////////////////
	void createFloor(CellID fill) {
		MewUserAssert(fill != -1, "cannot puts empty cell");
		Layer _floor;
		_floor.fill(width*height, fill);
		layers.push_back(_floor);
		r_texture.main = LoadRenderTexture(width*cell_size, height*cell_size);
		r_texture.sub  = LoadRenderTexture(width*cell_size, height*cell_size);
	}

	////////////////////////////////////////////////////////////
	void createLayer() {
		Layer _floor;
		_floor.fill(width*height, empty_cell);
		layers.push_back(_floor);
	}

	////////////////////////////////////////////////////////////
	void StepLayerUp(){
		if (++current_layer >= layers.size()) {
			current_layer = layers.size()-1;
		}
	}

	////////////////////////////////////////////////////////////
	void StepLayerDown() {
		if (--current_layer < 0) {
			current_layer = 0;
		}
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
		should_render = true;
	}

	////////////////////////////////////////////////////////////
	RenderTextures& render() {
		if (should_render) {
			MewAssert(current_storage != nullptr);
			Layer& l = layers[0];
			BeginTextureMode(r_texture.main);
			#pragma omp parallel for collapse(2)
			for (uint x = 0; x < width; ++x) {
				for (uint y = 0; y < height; ++y) {
					size_t index = mew::get_index(x, y, width);
					CellID cid = l.get(index);
					CellInfo* block = current_storage->get(cid);
					CellContext::Draw(x*cell_size, y*cell_size, block);
				}
			}
			EndTextureMode();
			should_render = false;
		}
		if (layers.size() == 1) { return r_texture; }

		Layer& ll = getCurrentLayer();
		BeginTextureMode(r_texture.sub);
		ClearBackground(ColorAlpha(BLACK, 0.0f));
		#pragma omp parallel for collapse(2)
		for (uint x = 0; x < width; ++x) {
			for (uint y = 0; y < height; ++y) {
				size_t index = mew::get_index(x, y, width);
				CellID cid = ll.get(index);
				if (cid == empty_cell) { continue; }
				CellInfo* block = current_storage->get(cid);
				CellContext::Draw(x*cell_size, y*cell_size, block, ll.get_dyn(index));
			}
		}
		EndTextureMode();
		return r_texture;
	}

	////////////////////////////////////////////////////////////
	RenderTextures& getRenderTexture() {
		return r_texture;
	}
	
	////////////////////////////////////////////////////////////
	void set(size_t x, size_t y, CellID idx, DynCellData data = nullptr) {
		getCurrentLayer().set(mew::get_index(x, y, width), idx, data);
		should_render = true;
	}

	////////////////////////////////////////////////////////////
	void put(size_t x, size_t y, CellID idx) {
		getCurrentLayer().set(mew::get_index(x, y, width), idx, nullptr);
	}
	
	////////////////////////////////////////////////////////////
	CellID& get(size_t x, size_t y) {
		return getCurrentLayer().get(mew::get_index(x, y, width));
	}
	// void dput() {

	// }
	
	////////////////////////////////////////////////////////////
	Layer& getCurrentLayer() {
		return layers[current_layer];	
	}

	////////////////////////////////////////////////////////////
	Vector2 bounds() {
		return Vector2{(float)width, (float)height};
	}
	
	////////////////////////////////////////////////////////////
	void clear() {
		UnloadRenderTexture(r_texture.main);
		UnloadRenderTexture(r_texture.sub);
	}
};

class KinematicBody {
public:
	float mass;
	vec2 force;
	vec2 velocity;
	vec2 position;
	float angular_velocity = 0.0f;
	float rotation = 0.0f;
	float target_rotation = 0.0f;

	KinematicBody() {}

	void applyForce(vec2 _force) {
		force += _force;
	}

	void applyForceX(float x) {
		force.x += x;
	}
	void applyForceY(float y) {
		force.y += y;
	}
	void setTargetRotation(float target_deg) {
		target_rotation = target_deg;
	}

	void move() {
		const float time_interval = GetFrameTime();
		// Update rotation
		rotation += angular_velocity * time_interval;
		
		// Adjust angular velocity to reach target rotation
		float rotation_diff = target_rotation - rotation;
		if (fabs(rotation_diff) < 5.0f) {
			angular_velocity = mew::lerp_value(angular_velocity, 0.0f, time_interval*10.0f);
		}
		if (fabs(rotation_diff) > 1.0f) {
			angular_velocity += rotation_diff * time_interval;
		} else {
			rotation = target_rotation;
		}

		if (force.isZero(0.1f)) { 
			velocity.lerp((vec2){0.0f, 0.0f}, time_interval);
		}
		if (force.isZero(0.01f)) { 
			force.zero();
			velocity.zero();
			velocity.lerp((vec2){0.0f, 0.0f}, time_interval);
			return;
		}
		vec2 acceleration = force / mass;
		vec2 delta_velocity = acceleration * time_interval; 
		position += velocity * time_interval;
		position += delta_velocity * time_interval / 2.0; 
		velocity += delta_velocity;
		force.lerp((vec2){0.0f, 0.0f}, time_interval);
	}

	const char* toString() {
		return TextFormat(
			"mass(%.2f), \n"
			"force{x:%f, y:%f}, \n"
			"velocity{x:%.2f, y:%.2f}, \n"
			"position{x:%.2f, y:%.2f}\n" 
			"rotation(%.0f)\n"
			"ang(%.2f)\n"
			"dt(%.2f)",
			mass, force.x, force.y, 
			velocity.x, velocity.y, 
			position.x, position.y,
			rotation, angular_velocity,
			GetFrameTime()*1000.0f
		);
	}

};

class Player {
public:
	KinematicBody body;
	Vector2 position = {0,0};
	Camera2D camera;
	CellID current_block;
	CellID player_cell;
	float x_speed = 400.0f;
	float y_speed = 400.0f;
	const char* name;
	Inventory inventory;

	
	Player(const char* name, CellID _player_cell) 
		: player_cell(_player_cell), inventory(name), name(name) {
		camera.rotation = 180.0f;
		camera.zoom = 2.0f;
		body.mass = 1.0f;
		body.position = {0.0f, 0.0f};
		body.velocity = {0.0f, 0.0f};
		body.force    = {0.0f, 0.0f};
	}


	void setPostion(float x, float y) {
		position.x = x;
		position.y = y;
		body.position.x = x;
		body.position.y = y;
	}

	void clear() {
		position.x = 0.0f;
		position.y = 0.0f;
		body.position.x = 0.0f;
		body.position.y = 0.0f;
		body.velocity.x = 0.0f;
		body.velocity.y = 0.0f;
		body.force.x = 0.0f;
		body.force.y = 0.0f;
		body.rotation = 0.0f;
	}

	void setBlock(CellID idx) {
		current_block = idx;
	}

	void zoomit() {
		camera.zoom += ((float)GetMouseWheelMove()*0.1f);
		camera.zoom = mew::clamp(camera.zoom, 1.0f, 3.0f);
		camera.offset = (Vector2){GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};
	}

	void Update() {
		const float time_interval = GetFrameTime();

		if(IsKeyPressed(current_data_set->RELOAD_KEY)) {
			current_data_set->load();
			// clear();
		}
		body.mass = current_data_set->PLAYER_MASS;
		float x_force = 25.0f;
		float y_force = 25.0f;
		if (IsKeyDown(current_data_set->PLAYER_CLUTCH)) {
			body.force.lerp((vec2){0.1f, 0.1f}, time_interval*10.0f);
			current_data_set->states.show_slutch_message = true;
		} else {
			current_data_set->states.show_slutch_message = false;
		}
		if (IsKeyDown(current_data_set->PLAYER_MOVE_RIGHT)) { /* right */
			body.force.x = -x_force;
			body.setTargetRotation(90.0f);
		}
		if (IsKeyDown(current_data_set->PLAYER_MOVE_LEFT)) { /* left */
			body.force.x = x_force;
			body.setTargetRotation(-90.0f);
		}
		if (IsKeyDown(current_data_set->PLAYER_MOVE_FRONT)) { /* front */
			body.force.y = y_force;
			body.setTargetRotation(0.0f);
		}
		if (IsKeyDown(current_data_set->PLAYER_MOVE_BACK)) { /* back */
			body.force.y = -y_force;
			body.setTargetRotation(180.0f);
		}
		body.move();
		position.x = body.position.x;
		position.y = body.position.y;
		camera.target = position;
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
		block->rotation = body.rotation;
		CellContext::Draw(position.x, position.y, block);
	}
};

class WorldContext {
public:
	static Vector2 GetCellPosByMouse(Camera2D& camera, World& w, bool* has_touches = nullptr) {
		Vector2 mouse = GetMousePosition();
		mouse = GetScreenToWorld2D(mouse, camera); // Convert screen space to world space
		Vector2 pos = w.getPos();
		Rectangle rect = w.getRect();
		Rectangle r(rect.x + pos.x, rect.y + pos.y, rect.width, rect.height);
		if (!CellContext::inRectangle(r, mouse))
		{
			if (has_touches != nullptr)
				*has_touches = false;
			return (Vector2){-1, -1}; // Return invalid position
		}
		if (has_touches != nullptr)
			*has_touches = true;

		// Convert world space to cell coordinates
		mouse.x = (mouse.x - pos.x) / cell_size;
		mouse.y = (mouse.y - pos.y) / cell_size;

		// Ensure the cell coordinates are within valid bounds
		mouse.x = floorf(mouse.x);
		mouse.y = floorf(mouse.y) + 1;

		return mouse;
	}

	static void PutBlock(World& w, Player& p) {
		bool has_touches;
		Vector2 cell_pos = GetCellPosByMouse(p.camera, w, &has_touches);
		if (!has_touches) { return; }
		CellInfo* block = current_storage->get(p.current_block);
		floor_particle_system->spawn("put_block", (vec2){cell_pos.x*cell_size, (cell_pos.y-1)*cell_size}, 8.0f);
		cell_pos.y = w.height - cell_pos.y;
		w.put(cell_pos.x, cell_pos.y, p.current_block);
	}

	static void DestroyBlock(World& w, Player& p) {
		bool has_touches;
		Vector2 cell_pos = GetCellPosByMouse(p.camera, w, &has_touches);
		if (!has_touches) { return; }
		CellInfo* block = current_storage->get(p.current_block);
		floor_particle_system->spawn("destroy_block", (vec2){cell_pos.x*cell_size, (cell_pos.y-1)*cell_size}, 7.0f);
		cell_pos.y = w.height - cell_pos.y;
		w.put(cell_pos.x, cell_pos.y, empty_cell);
	}
	
	static void Update(World& w, Player& p) {
		MewAssert(floor_particle_system != nullptr);
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
			PutBlock(w, p);
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			DestroyBlock(w, p);
		}
		floor_particle_system->update();
	}
	
	static void Render(World& w) {
		w.render();
	}

	static void Draw(World& w) {
		MewAssert(floor_particle_system != nullptr);
		auto _texture = w.getRenderTexture();
		DrawTextureRec(_texture.main.texture, w.getRect(), w.getPos(), WHITE);
		floor_particle_system->render(w.getPos());
		DrawTextureRec(_texture.sub.texture, w.getRect(), w.getPos(), WHITE);
		if (top_particle_system != nullptr) {
			top_particle_system->render(w.getPos());
		}
	}
};


#endif