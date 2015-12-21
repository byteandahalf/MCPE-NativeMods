#pragma once
#include <memory>

struct Inventory;
struct ItemInstance;
struct Level;

struct Player {
	char filler[116]; // 0
	Level& level; // 116
	char filler2[3328]; // 120
	std::unique_ptr<Inventory> inventory; // 3448
	
	void drop(const ItemInstance*);
};

