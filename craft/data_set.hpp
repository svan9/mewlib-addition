#ifndef DATA_SET
#define DATA_SET

#include "mewall.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#pragma pack(push, 1)

struct DataSetStates {
	byte show_slutch_message: 1 = false;
	byte ui_focused: 1 = false;
};

#pragma pack(pop)

struct DataSet {
	const char* file = "resources/settings.json";
	DataSetStates states;
	int RELOAD_KEY;
	int PLAYER_MOVE_FRONT;
	int PLAYER_MOVE_BACK;
	int PLAYER_MOVE_LEFT;
	int PLAYER_MOVE_RIGHT;
	int PLAYER_CLUTCH;
	float PLAYER_MASS;
	
	void load() {
		std::ifstream f(file);
		json data = json::parse(f);
		// keys
		std::string __RELOAD_KEY = data["RELOAD_KEY"].get<std::string>();
		std::string __PLAYER_MOVE_FRONT = data["PLAYER_MOVE_FRONT"].get<std::string>();
		std::string __PLAYER_MOVE_BACK = data["PLAYER_MOVE_BACK"].get<std::string>();
		std::string __PLAYER_MOVE_LEFT = data["PLAYER_MOVE_LEFT"].get<std::string>();
		std::string __PLAYER_MOVE_RIGHT = data["PLAYER_MOVE_RIGHT"].get<std::string>();
		std::string __PLAYER_CLUTCH = data["PLAYER_CLUTCH"].get<std::string>();
		// update key enum value
		data.at(__RELOAD_KEY).get_to(RELOAD_KEY);
		data.at(__PLAYER_MOVE_FRONT).get_to(PLAYER_MOVE_FRONT);
		data.at(__PLAYER_MOVE_BACK).get_to(PLAYER_MOVE_BACK);
		data.at(__PLAYER_MOVE_LEFT).get_to(PLAYER_MOVE_LEFT);
		data.at(__PLAYER_MOVE_RIGHT).get_to(PLAYER_MOVE_RIGHT);
		data.at(__PLAYER_CLUTCH).get_to(PLAYER_CLUTCH);
		// numeric constant
		data["PLAYER_MASS"].get_to(PLAYER_MASS);
		f.close();
	}
};


inline DataSet* current_data_set = nullptr; 
DataSet* getDataSet() {
	if (current_data_set == nullptr) {
		current_data_set = new DataSet();
	}
	return current_data_set;
} 

#endif