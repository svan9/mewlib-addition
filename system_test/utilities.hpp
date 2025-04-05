#ifndef UTILS_HPP
#define UTILS_HPP
extern "C" {
	#include "raylib.h"
	#include "rlgl.h"
}
#include "raymath.h"
#include "mewall.h"
#include <unordered_map>
#include <vector>

float GetDistance(Vector2 a, Vector2 b) {
	return Vector2Length(Vector2Subtract(a, b));
}

Image LoadImageEx(Image img, Rectangle source) {
	Image result = GenImageColor(source.width, source.height, RED);
	ImageDraw(&result, img, (Rectangle){0,0,source.width,source.height}, source, WHITE);
	return result;
}

void DrawCircleGradient(int centerX, int centerY, float radius, float inner_radius, Color inner, Color outer) {
	rlBegin(RL_TRIANGLES);
		for (int i = 0; i < 360; i += 10) {
			/* inner first */
			rlColor4ub(inner.r, inner.g, inner.b, inner.a);
			rlVertex2f((float)centerX + cosf(DEG2RAD*i)*inner_radius, (float)centerY + sinf(DEG2RAD*i)*inner_radius);
			/* inner second */
			rlColor4ub(inner.r, inner.g, inner.b, inner.a);
			rlVertex2f((float)centerX + cosf(DEG2RAD*(i + 10))*inner_radius, (float)centerY + sinf(DEG2RAD*(i + 10))*inner_radius);
			/* outer second */
			rlColor4ub(outer.r, outer.g, outer.b, outer.a);
			rlVertex2f((float)centerX + cosf(DEG2RAD*i)*radius, (float)centerY + sinf(DEG2RAD*i)*radius);
			/* outer first */
			rlColor4ub(outer.r, outer.g, outer.b, outer.a);
			rlVertex2f((float)centerX + cosf(DEG2RAD*(i + 10))*radius, (float)centerY + sinf(DEG2RAD*(i + 10))*radius);
			/* outer second */
			rlColor4ub(outer.r, outer.g, outer.b, outer.a);
			rlVertex2f((float)centerX + cosf(DEG2RAD*i)*radius, (float)centerY + sinf(DEG2RAD*i)*radius);
			/* inner second */
			rlColor4ub(inner.r, inner.g, inner.b, inner.a);
			rlVertex2f((float)centerX + cosf(DEG2RAD*(i + 10))*inner_radius, (float)centerY + sinf(DEG2RAD*(i + 10))*inner_radius);
		}
	rlEnd();
}
using mew::vec2;

float GetRandomValue(float from, float to) {
	return from + (to-from)*(float)GetRandomValue(0, INT_MAX) / INT_MAX;
}

struct Bound {
	vec2 min, max;
};

vec2 getRandom(Bound min, Bound max) {
	vec2 pos;
	while (pos.x > max.min.x && pos.x < max.max.x)
		pos.x = GetRandomValue(min.min.x, min.max.x);
	while (pos.y > max.min.y && pos.y < max.max.y)
		pos.y = GetRandomValue(min.min.y, min.max.y);
	return pos;
}

vec2 getRandom(Bound b) {
	vec2 pos;
	pos.x = GetRandomValue(b.min.x, b.max.x);
	pos.y = GetRandomValue(b.min.y, b.max.y);
	return pos;
}

vec2 getRandom(Rectangle min_rect, Rectangle max_rect) {
	return getRandom(
		(Bound){(vec2){min_rect.x, min_rect.y}, 
			(vec2){min_rect.x+min_rect.width, min_rect.y+min_rect.height}},
		(Bound){(vec2){max_rect.x, max_rect.y}, 
			(vec2){max_rect.x+max_rect.width, max_rect.y+max_rect.height}});
}

vec2 getRandom(Rectangle rect) {
	return getRandom((Bound){
		(vec2){rect.x, rect.y}, 
		(vec2){rect.x+rect.width, rect.y+rect.height}});
}



// Vector2 operator+(const Vector2& v1, const Vector2& v2) {
// 	return (Vector2){v1.x+v2.x, v1.y+v2.y};
// }
// Vector2 operator-(const Vector2& v1, const Vector2& v2) {
// 	return (Vector2){v1.x-v2.x, v1.y-v2.y};
// }
// Vector2 operator*(const Vector2& v1, const Vector2& v2) {
// 	return (Vector2){v1.x*v2.x, v1.y*v2.y};
// }
// Vector2 operator/(const Vector2& v1, const Vector2& v2) {
// 	return (Vector2){v1.x/v2.x, v1.y/v2.y};
// }
// Vector2 operator+(const Vector2& v1, float v2) {
// 	return (Vector2){v1.x+v2, v1.y+v2};
// }
// Vector2 operator-(const Vector2& v1, float v2) {
// 	return (Vector2){v1.x-v2, v1.y-v2};
// }
// Vector2 operator*(const Vector2& v1, float v2) {
// 	return (Vector2){v1.x*v2, v1.y*v2};
// }
// Vector2 operator/(const Vector2& v1, float v2) {
// 	return (Vector2){v1.x/v2, v1.y/v2};
// }
// Vector2 operator+(Vector2&& v1, Vector2&& v2) {
// 	return (Vector2){v1.x+v2.x, v1.y+v2.y};
// }
// Vector2 operator-(Vector2&& v1, Vector2&& v2) {
// 	return (Vector2){v1.x-v2.x, v1.y-v2.y};
// }
// Vector2 operator*(Vector2&& v1, Vector2&& v2) {
// 	return (Vector2){v1.x*v2.x, v1.y*v2.y};
// }
// Vector2 operator/(Vector2&& v1, Vector2&& v2) {
// 	return (Vector2){v1.x/v2.x, v1.y/v2.y};
// }
// Vector2 operator+(Vector2&& v1, float v2) {
// 	return (Vector2){v1.x+v2, v1.y+v2};
// }
// Vector2 operator-(Vector2&& v1, float v2) {
// 	return (Vector2){v1.x-v2, v1.y-v2};
// }
// Vector2 operator*(Vector2&& v1, float v2) {
// 	return (Vector2){v1.x*v2, v1.y*v2};
// }
// Vector2 operator/(Vector2&& v1, float v2) {
// 	return (Vector2){v1.x/v2, v1.y/v2};
// }

Rectangle GetTextBounds(const char* text, float font_size) {
	int textWidth = MeasureText(text, font_size);
	int textHeight = font_size;
	return (Rectangle){ 0, 0, (float)textWidth, (float)textHeight };
}

Rectangle operator+(const Rectangle& r1, const Vector2& v) {
	return (Rectangle){r1.x+v.x, r1.y+v.y, r1.width, r1.height};
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