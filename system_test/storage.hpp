#ifndef STORAGE_HPP
#define STORAGE_HPP

#include "mewall.h"
#include "utilities.hpp"
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <initializer_list>

using mew::vec2;

const auto& same_s = mew::string::SameStr;
extern "C" {
	#include "raylib.h"
}

typedef size_t CellID;

enum AnimationRepeat: byte {
	Repeat,
	NoRepeat,
	Reverse,
	NoRepeatReverse
};

struct CellAnimationInfo {
	size_t count; // count of frames
	float speed;  // speed of animation(seconds)
	AnimationRepeat repeat;
};

struct TileInfo {
	const char* name;
	CellID idx;
	bool flip_x: 1 = false;
	bool flip_y: 1 = false;
	CellAnimationInfo* animation = nullptr;
};

struct Atlas {
	Image image;
	Vector2 tile_size;
	Vector2 count;
	Texture2D texture;
};

class TextureAtlas {
public:
	typedef TextureAtlas This;
	////////////////////////////////////////////////////////////
	Atlas* atlas = nullptr;
	std::vector<TileInfo> cells_info;

	////////////////////////////////////////////////////////////
	TextureAtlas() {	}

	////////////////////////////////////////////////////////////
	TextureAtlas& add(TileInfo& info) {
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
		return (CellID)0;
	}

	////////////////////////////////////////////////////////////
	TileInfo get(const char* name) {
		for (auto& cell: cells_info) {
			if (same_s(cell.name, name)) {
				return cell;
			}
		}
		return (TileInfo)0;
	}

	////////////////////////////////////////////////////////////
	TileInfo get(CellID id) {
		if (!id >= cells_info.size()) {
			return (TileInfo)0;
		}
		return cells_info[id];
	}

	////////////////////////////////////////////////////////////
	This& loadAtlas(const char* path, float tile_width, float tile_height) {
		atlas = new Atlas();
		Image image = LoadImage(path);
		Texture2D texture = LoadTextureFromImage(image);  
		atlas->image = image;
		UnloadImage(image);
		atlas->tile_size = {tile_width, tile_height};
		atlas->count = {texture.width / tile_width, texture.height / tile_height};
		atlas->texture = texture;
		return *this;
	}

	////////////////////////////////////////////////////////////
	This& upload(CellID id, const char* name, bool flip_x = false, bool flip_y = false) {
		TileInfo info; 
		info.idx = id-1;
		info.name = name;
		info.flip_x = flip_x;
		info.flip_y = flip_y;
		cells_info.push_back(info);
		return *this;
	}

	////////////////////////////////////////////////////////////
	This& addAnimation(const char* name, size_t count, float speed = 1.0f, AnimationRepeat repeat = Repeat) {
		auto cell = get(name);
		cell.animation = new CellAnimationInfo();
		cell.animation->count = count;
		cell.animation->speed = speed;
		cell.animation->repeat = repeat;
		return *this;
	}

	////////////////////////////////////////////////////////////
	void draw(TileInfo& info, Vector2 pos, Vector2 size, Color tint) {
		MewUserAssert(atlas != nullptr, "atlas is not loaded");
		Rectangle source = {info.idx * atlas->tile_size.x, 0, atlas->tile_size.x, atlas->tile_size.y};
		Rectangle dest;
		if (size == (Vector2){0,0}) {
			dest = {pos.x, pos.y, atlas->tile_size.x, atlas->tile_size.y};
		} else {
			dest = {pos.x, pos.y, size.x, size.y};
		}
		DrawTexturePro(atlas->texture, source, dest, {0, 0}, 0, tint);
	}

	////////////////////////////////////////////////////////////
	void draw(const char* name, Vector2 pos, Vector2 size = {0,0}, Color tint = WHITE) {
		MewUserAssert(atlas != nullptr, "atlas is not loaded");
		TileInfo info = get(name);
		auto idx = mew::get_index(info.idx, atlas->count.x);
		Rectangle source = {idx.first * atlas->tile_size.x, idx.second*atlas->tile_size.y, atlas->tile_size.x, atlas->tile_size.y};
		Rectangle dest;
		if (size == (Vector2){0,0}) {
			dest = {pos.x, pos.y, atlas->tile_size.x, atlas->tile_size.y};
		} else {
			dest = {pos.x, pos.y, size.x, size.y};
		}
		float rotation = 
			(info.flip_x ? 180 : 0) + (info.flip_y ? 90 : 0);
		DrawTexturePro(atlas->texture, source, dest, {0, 0}, rotation, tint);
	}
};

static TextureAtlas* __current_atlas = nullptr;
TextureAtlas* GetCurrentTextureAtlas() {
	if (__current_atlas == nullptr) {
		__current_atlas = new TextureAtlas();
	}
	return __current_atlas;
}



#endif