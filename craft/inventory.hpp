#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <vector>
#include "ui.hpp"

#pragma pack(push, 1)

struct Item;

typedef void(*ItemProcessor)(Item&);

struct Item {
	size_t idx;
	Image image;
	Texture2D texture;
	size_t max_stack = -1;
	ItemProcessor proc;
};

inline std::unordered_map<const char*, Item> items_register;

Item& GetItem(const char* name) {
	return items_register.at(name);	
}

Item& CreateItem(const char* name, Image img, size_t max_stack = -1, ItemProcessor proc = nullptr) {
	Item item;
	item.idx = items_register.size();
	item.image = img;
	item.texture = LoadTextureFromImage(img);
	item.max_stack = max_stack;
	item.proc = proc;
	items_register.insert({name, item});
	return items_register.at(name);
}

struct ItemStack { 
	Item item;
	size_t count;
	std::string nbt;
};

bool operator==(const Item& l, const Item& r) {
	return l.idx == r.idx;
}

void swap(ItemStack& l, ItemStack& r) {
	std::swap(l.item, r.item);
	std::swap(l.count, r.count);
	std::swap(l.nbt, r.nbt);
}

struct InventoryCell { 
	ItemStack stack;
	size_t max_stack_size = -1;
	bool can_put: 1 = true;
	bool can_grab: 1 = true;
};

inline InventoryCell empty_inv_cell = (InventoryCell){
	.stack = (ItemStack){.count = 0}
};

struct MouseGrabedItemStack {
	ItemStack stack;
	size_t cell_idx;
	size_t target_cell_idx;
};

class Inventory {
private:
	UI& ui;
	UI cell_ui;
	const char* name;
	bool is_showed = false;
	MouseGrabedItemStack mouse_grabbed;
	std::vector<InventoryCell> cells;
public:
	Inventory(const char* name)
		: ui(GetUICluster()->create(name)), name(name) {}

	Inventory(const char* name, size_t size)
		: ui(GetUICluster()->create(name)), name(name) {
		cells.resize(size, empty_inv_cell);		
	}
	
	void initUI() { }

	void init() {
		initUI();
	}

	Rectangle getCellRect() {
		return cell_ui.getRect(ui.getRectByScreen());
	}

	size_t getCellIdxTouched() {
		for (size_t i = 0; i < cells.size(); ++i) {
			Rectangle rect = getCellRect();
			if (CheckCollisionPointRec(GetMousePosition(), rect)) {
				return i;
			}
		}
		return (size_t)-1;
	}
	
	void updateSyncStacks() {
		for (auto& cell: cells) {
			cell.stack.count = 
				mew::clamp(cell.stack.count, 0, 
					std::min(cell.max_stack_size, cell.stack.item.max_stack));
		}	
	}

	void updateMouse() {
		if (getDataSet()->states.item_grab) {
			size_t target_idx = getCellIdxTouched();
			if (target_idx != (size_t)-1) {
				mouse_grabbed.target_cell_idx = target_idx;  
			}
		}
	}
	
	void UpdateItemGrabing() {
		if (getDataSet()->states.item_finish_grab) {
			auto& target_cell = cells[mouse_grabbed.target_cell_idx];
			auto& from_cell = cells[mouse_grabbed.cell_idx];
			if (target_cell.stack.item == from_cell.stack.item) {
				size_t max_stack = std::min(target_cell.max_stack_size, target_cell.stack.item.max_stack);
				if (target_cell.stack.count + mouse_grabbed.stack.count > max_stack) {
					int32_t limitted = max_stack - mouse_grabbed.stack.count;
					target_cell.stack.count = max_stack;
					from_cell.stack.count -= limitted; 
				}
				getDataSet()->states.item_finish_grab = false;
			} else {
				swap(target_cell.stack, from_cell.stack);
				getDataSet()->states.item_finish_grab = false;
			}
		}
	}
	
	void update() {
		updateMouse();
		UpdateItemGrabing();
		updateSyncStacks();
	}

	void render() {
		for (size_t i = 0; i < cells.size(); ++i) {
			Rectangle rect = getCellRect();
			/* draw border with background */
			// todo background with image;
			DrawRectangle(rect.x, rect.y, rect.width, rect.height, cell_ui.style.background.color);
			// todo using border width;
			DrawRectangleLines(
				rect.x,
				rect.y,
				rect.width,
				rect.height,
				cell_ui.style.border.color);
			/* draw item count */
			DrawTexture(cells[i].stack.item.texture, rect.x, rect.y, WHITE);
			const char* text = TextFormat("%i", cells[i].stack.count);
			auto measure = GetTextBounds(text, 15);
			DrawText(text, rect.x+rect.width-measure.width, rect.y+rect.height-measure.height, 15, BLACK);
		}
		if (getDataSet()->states.item_grab) {
			Vector2 mouse_pos = GetMousePosition();
			DrawTextureV(mouse_grabbed.stack.item.texture, mouse_pos, WHITE);
		}
	}
};

#pragma pack(pop)

#endif // INVENTORY_HPP