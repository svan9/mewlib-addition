#ifndef UI_HPP
#define UI_HPP
extern "C" {
	#include "raylib.h"
}
#include "mewall.h"
#include <unordered_map>
#include <vector>
#include "utilities.hpp"
#include "style.hpp"

class UI {
public:
	typedef void(*Proccessor)(UI&);
	ElementStyle style;
	std::vector<UI> children;
	Proccessor onHover = nullptr;
	Proccessor onClick = nullptr;
	UI() {}

	Rectangle getRect(Rectangle rect) {
		return style.getAbsoluteRect(rect);
	}

	Rectangle getRectByScreen() {
		return style.getAbsoluteRect((Rectangle){0,0, (float)GetScreenWidth(), (float)GetScreenHeight()});
	}

	void update() {
		Rectangle _rect = getRectByScreen();
		if (CheckCollisionPointRec(GetMousePosition(), _rect)) {
			_onHover();
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				_onClick();
			}
		}
		for (auto& child: children) {
			child.update();
		}
	}

	void render(Rectangle rect) {
		Rectangle _rect = getRect(rect);
		Rectangle current_rect = _rect;
		for (auto& child: children) {
			Rectangle child_rect = child.style.getAbsoluteRect(current_rect);
			if (style.display == DisplayStyle::Flex) {
				if (style.flex_direction == FlexDirectionStyle::Row) {
					current_rect.x += child_rect.width;
					if (current_rect.x > _rect.width) {
						current_rect.x = _rect.x;
						current_rect.y += child_rect.height;
					}
				}
				if (style.flex_direction == FlexDirectionStyle::Column) {
					current_rect.y += child_rect.height;
					if (current_rect.y > _rect.height) {
						current_rect.y = _rect.y;
						current_rect.x += child_rect.width;
					}
				}
			}
			child.render(current_rect+(Vector2){_rect.x, _rect.y});
		}
		if (style.background.has_image) {
			style.background.texture.width = _rect.width;
			style.background.texture.height = _rect.height;
			DrawTexture(style.background.texture, _rect.x, _rect.y, style.disabled ? GRAY: WHITE);
		} else {
			DrawRectangleRec(_rect, style.background.color);
		}
		if (style.text != nullptr) {
			int width = MeasureText(style.text, style.font_size);
			DrawText(style.text, _rect.x+(_rect.width/2)-(width/2), 
				_rect.y+(_rect.height/2)-(style.font_size/2), style.font_size, style.text_color);
		} 
	}

	void _onHover() {
		if (style.disabled) return;
		if (onHover!=nullptr) onHover(*this);
		for (auto& child: children) {
			child._onHover();
		}
	}

	void _onClick() {
		if (style.disabled) return;
		if (onClick!=nullptr) onClick(*this);
		for (auto& child: children) {
			child._onClick();
		}
	}

	void add(UI& ui) {
		children.push_back(ui);
	}
};

class UICluster {
public:
	typedef std::unordered_map<const char*, UI> cluster_t;
private:
	cluster_t cluster;
	size_t focused_element;
public:
	////////////////////////////////////////////////////////////
	UICluster() {}

	////////////////////////////////////////////////////////////
	UI& append(const char* name, UI& ui) {
		cluster.insert({name, ui});
		return cluster.at(name);
	}
	
	////////////////////////////////////////////////////////////
	UI& create(const char* name) {
		UI __temp;
		cluster.insert({name, __temp});
		return cluster.at(name);
	}

	////////////////////////////////////////////////////////////
	bool has(const char* name) {
		return cluster.find(name) != cluster.end();
	}

	////////////////////////////////////////////////////////////
	UI& get(const char* name) {
		if (!has(name)) {
			return create(name);
		}
		return cluster.at(name);
	}
	
	////////////////////////////////////////////////////////////
	cluster_t& list() noexcept {
		return cluster;
	}

	////////////////////////////////////////////////////////////
	void show(const char* name) { 
		UI& __e = get(name);
		__e.style.is_show = true;
	}

	////////////////////////////////////////////////////////////
	void hide(const char* name) { 
		UI& __e = get(name);
		__e.style.is_show = false;
	}

	////////////////////////////////////////////////////////////
	void blur() { 
		for (auto& __e: cluster) {
			__e.second.style.is_focus = false;
		}
	}
	
	////////////////////////////////////////////////////////////
	void focus(const char* name) { 
		blur();
		UI& __e = get(name);
		__e.style.is_focus = true;
	}

	////////////////////////////////////////////////////////////
	void blur(const char* name) { 
		UI& __e = get(name);
		__e.style.is_focus = false;
	}

	////////////////////////////////////////////////////////////
	void update() {
		for (auto& __e: cluster) {
			__e.second.update();
		}
	}

	////////////////////////////////////////////////////////////
	void render() {
		for (auto& __e: cluster) {
			if (__e.second.style.is_show) {
				__e.second.render((Rectangle){0,0, (float)GetScreenWidth(), (float)GetScreenHeight()});
			}
		}
	}
};

UICluster* main_ui_cluster = nullptr;
UICluster* GetUICluster() {
	if (main_ui_cluster == nullptr) {
		main_ui_cluster = new UICluster();
	}
	return main_ui_cluster;
}




#endif