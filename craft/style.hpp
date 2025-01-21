#ifndef STYLE_HPP
#define STYLE_HPP
extern "C" {
  #include "raylib.h"
}
#include "mewall.h"
#include <unordered_map>
#include <vector>
#include "data_set.hpp"
#include "utilities.hpp"

#pragma pack(push, 1)

enum struct Alignment: byte {
  Floating,
  Center,
  TopLeft,
  MiddleLeft,
  BottomLeft,
  TopRight,
  MiddleRight,
  BottomRight,
  MiddleBottom,
  MiddleTop
};

struct Background {
  bool has_image = false;
  Color color = BLACK;
  Image image;
};

enum struct DisplayStyle: byte {
  Normal, Flex  
};

enum struct FlexDirectionStyle: byte {
  Row, Column
};

struct SemiVec {
  bool has_percent = false;
  Vector2 value;

  Vector2 getAbsolute(Vector2 size) {
    if (!has_percent) { return value; }
    return (Vector2){value.x*size.x, value.y*size.y};
  }
};

struct Margin {
  float left    = 0;
  float right   = 0;
  float top     = 0;
  float bottom  = 0;
};

struct ElementStyle {
  bool is_show: 1 = false;
  bool is_focus: 1 = false;
  Alignment box_align = Alignment::MiddleLeft;
  Alignment text_align = Alignment::MiddleLeft;
  Color text_color = BLACK;
  const char* text = nullptr;
  Background background;
  DisplayStyle display = DisplayStyle::Flex;
  FlexDirectionStyle flex_direction = FlexDirectionStyle::Row;
  SemiVec position;
  SemiVec size;
  Margin margin;

  bool hasText() {
    return text != nullptr;
  }
  
  Rectangle getAbsoluteRectNoAlign(Rectangle byRect) {
    return (Rectangle)(
      position.getAbsolute((Vector2){byRect.x, byRect.y}) <<
      size.getAbsolute((Vector2){byRect.width, byRect.height})
    );
  }

  Rectangle getAbsoluteRectNoMarginNPadding(Rectangle byRect) {
    switch(box_align) {
      case Alignment::Floating: {
        return getAbsoluteRectNoAlign((Rectangle){byRect.x, byRect.y, byRect.width, byRect.height});
      } break;
      case Alignment::Center: {
        return getAbsoluteRectNoAlign((Rectangle){byRect.x, byRect.y, byRect.width, byRect.height});
      } break;
      case Alignment::MiddleLeft: {
        return getAbsoluteRectNoAlign((Rectangle){1, byRect.y, byRect.width, byRect.height});
      } break;
      case Alignment::TopLeft: {
        return getAbsoluteRectNoAlign((Rectangle){0, 0, byRect.width, byRect.height});
      } break;
      case Alignment::MiddleTop: {
        return getAbsoluteRectNoAlign((Rectangle){byRect.x, 1, byRect.width, byRect.height});
      } break;
      case Alignment::TopRight: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){1,1, byRect.width, byRect.height});
        rect.x = byRect.width-rect.width;
        return rect;
      } break;
      case Alignment::MiddleRight: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){1,byRect.y, byRect.width, byRect.height});
        rect.x = byRect.width-rect.width;
        return rect;
      } break;
      case Alignment::BottomRight: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){1, 1, byRect.width, byRect.height});
        rect.x = byRect.width-rect.width;
        rect.y = byRect.height-rect.height;
        return rect;
      } break;
      case Alignment::MiddleBottom: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){1, 1, byRect.width, byRect.height});
        rect.y = byRect.height-rect.height;
        rect.x = byRect.width - (rect.width/2.0f);
        return rect;
      } break;
      case Alignment::BottomLeft: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){1, 1, byRect.width, byRect.height});
        rect.y = byRect.height-rect.height;
        return rect;
      } break;
      default: MewUserAssert(false, "undefined alignment for box");
    }
  }
  
  Rectangle getAbsoluteRect(Rectangle byRect) {
    switch(box_align) {
      case Alignment::Floating: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){byRect.x, byRect.y, byRect.width, byRect.height});
        rect.x += margin.left;
        rect.x -= margin.right;
        rect.y += margin.top;
        rect.y -= margin.bottom;
        return rect;
      } break;

      case Alignment::Center: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){byRect.x, byRect.y, byRect.width, byRect.height});
        return rect;
      } break;
      case Alignment::BottomLeft: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){1, 1, byRect.width, byRect.height});
        rect.y = byRect.height-rect.height;
        rect.x += margin.left;
        rect.y -= margin.bottom;
        return rect;
      } break;
      case Alignment::MiddleLeft: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){1, byRect.y, byRect.width, byRect.height});
        rect.x += margin.left;
        return rect;
      } break;
      case Alignment::TopLeft: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){0, 0, byRect.width, byRect.height});
        rect.x += margin.left;
        rect.y += margin.top;
        return rect;
      } break;
      case Alignment::MiddleTop: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){byRect.x, 1, byRect.width, byRect.height});
        rect.y += margin.top;
        return rect;
      } break;
      case Alignment::TopRight: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){1,1, byRect.width, byRect.height});
        rect.x = byRect.width-rect.width;
        rect.x -= margin.right;
        rect.y += margin.top;
        return rect;
      } break;
      case Alignment::MiddleRight: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){1,byRect.y, byRect.width, byRect.height});
        rect.x = byRect.width-rect.width;
        rect.x -= margin.right;
        return rect;
      } break;
      case Alignment::BottomRight: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){1, 1, byRect.width, byRect.height});
        rect.x = byRect.width-rect.width;
        rect.y = byRect.height-rect.height;
        rect.x += margin.right;
        rect.y -= margin.bottom;
        return rect;
      } break;
      case Alignment::MiddleBottom: {
        auto rect = getAbsoluteRectNoAlign((Rectangle){1, 1, byRect.width, byRect.height});
        rect.y = byRect.height-rect.height;
        rect.x = byRect.width - (rect.width/2.0f);
        rect.y -= margin.bottom;
        return rect;
      } break;
      default: MewUserAssert(false, "undefined alignment for box");
    }
  }
};

#pragma pack(pop)

#endif