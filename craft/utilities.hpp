#ifndef UTILS_HPP
#define UTILS_HPP
extern "C" {
	#include "raylib.h"
}
#include "mewall.h"
#include <unordered_map>
#include <vector>


Vector2 operator+(const Vector2& v1, const Vector2& v2) {
	return (Vector2){v1.x+v2.x, v1.y+v2.y};
}
Vector2 operator-(const Vector2& v1, const Vector2& v2) {
	return (Vector2){v1.x-v2.x, v1.y-v2.y};
}
Vector2 operator*(const Vector2& v1, const Vector2& v2) {
	return (Vector2){v1.x*v2.x, v1.y*v2.y};
}
Vector2 operator/(const Vector2& v1, const Vector2& v2) {
	return (Vector2){v1.x/v2.x, v1.y/v2.y};
}
Vector2 operator+(const Vector2& v1, float v2) {
	return (Vector2){v1.x+v2, v1.y+v2};
}
Vector2 operator-(const Vector2& v1, float v2) {
	return (Vector2){v1.x-v2, v1.y-v2};
}
Vector2 operator*(const Vector2& v1, float v2) {
	return (Vector2){v1.x*v2, v1.y*v2};
}
Vector2 operator/(const Vector2& v1, float v2) {
	return (Vector2){v1.x/v2, v1.y/v2};
}

Rectangle GetTextBounds(const char* text, float font_size) {
	int textWidth = MeasureText(text, font_size);
	int textHeight = font_size;
	return (Rectangle){ 0, 0, (float)textWidth, (float)textHeight };
}

inline Vector2 text_selection = (Vector2){0,0};

void InputTextEx(
	char* buffer, size_t buffer_size, bool* text_focused, 
	Vector2 pos, Vector2 size, 
	float font_size,
	Color background, Color text
) {
	// DrawRectangleV(pos-2.0f, size+4.0f, text);
	DrawRectangleV(pos, size, background);
	double time = mew::getTimeSeconds();
	bool show_cursor = (bool)((size_t)time % 2);
	Rectangle bounds = GetTextBounds(buffer, font_size);
	Vector2 cursor_pos = pos+(Vector2){0, bounds.width};
	Vector2 cursor_size = (Vector2){bounds.height, font_size*0.5f};
	DrawRectangleV(cursor_pos, cursor_size, text);
	DrawText(buffer, pos.x, pos.y, font_size, text);
	size_t length = strlen(buffer);
	bool collision = CheckCollisionPointRec(GetMousePosition(), (Rectangle){pos.x, pos.y, size.x, size.y});
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		*text_focused = collision;
	}
	if (*text_focused) {
		SetMouseCursor(MOUSE_CURSOR_IBEAM);
		int key = GetCharPressed();
		while (key > 0) {
			if ((key >= 32) && (key <= 125) && (length < buffer_size-1)) {
				buffer[length] = (char)key;
				buffer[length+1] = '\0';
				length++;
			}
			key = GetCharPressed();
		}		
	} else {
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
	}
}

void InputText(
	char* buffer, size_t buffer_size, bool* text_focused,
	float posx, float posy, 
	float font_size, 
	float width, float height,
	Color background, Color text
) {
	Vector2 pos = (Vector2){posx, posy};
	Vector2 size = (Vector2){width,height}; 
	return InputTextEx(buffer, buffer_size, text_focused, pos, size, font_size, background, text);
}

Rectangle operator<<(const Vector2& pos, const Vector2& size) {
	return (Rectangle){pos.x, pos.y, size.x, size.y};
}


#endif