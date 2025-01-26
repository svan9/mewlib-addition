#ifndef STORAGE_HPP
#define STORAGE_HPP

#include "mewall.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <initializer_list>

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



#endif