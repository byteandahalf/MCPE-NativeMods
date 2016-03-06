#pragma once
#include <memory>

struct Inventory;
struct ItemInstance;
struct Level;

struct Player {
	char filler2[3480]; // 120
	std::unique_ptr<Inventory> inventory; // 3440
	
	void drop(const ItemInstance*);
};

