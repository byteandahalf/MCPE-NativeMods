#pragma once
struct MinecraftClient;

struct Gui {
     char filler[2588]; // 0
     MinecraftClient& minecraft; // 2588
	 
	 int getSlotIdAt(int, int) const;
	 void _tickItemDrop();
};

