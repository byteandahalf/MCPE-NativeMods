#pragma once
struct Item;
struct Block;
struct CompoundTag;

struct ItemInstance {
	uint8_t count; // 0
	short data; // 2
	CompoundTag* userData; // 4
	bool valid; // 8
	Item* item; // 12
	Block* block; // 16
};

