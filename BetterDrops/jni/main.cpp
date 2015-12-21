#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <stdlib.h>
#include <string>
#include "substrate.h"

#include "mcpe/Gui.h"
#include "mcpe/MinecraftClient.h"
#include "mcpe/MenuPointer.h"
#include "mcpe/Player.h"
#include "mcpe/Inventory.h"
#include "mcpe/ItemInstance.h"

struct DropDelay {
	int8_t _delay;
	
	DropDelay() {
		_delay = 15;
	}
	
	const DropDelay& operator=(const int& delay) {
		_delay = delay;
		return *this;
	}
	
	const DropDelay& operator--() {
		if(_delay > 1)
			--_delay;
			
		return *this;
	}
	
	operator int() const {
		return _delay;
	}
};

static int slotTick = 15;
static DropDelay delay;
static bool stop = false;

void (*_Gui$_tickItemDrop)(Gui*);
void Gui$_tickItemDrop(Gui* gui) {
	if(MenuPointer::isPressed())
	{
		int touchedSlot = gui->getSlotIdAt(MenuPointer::getX(), MenuPointer::getY());
		if(touchedSlot == gui->minecraft.getLocalPlayer()->inventory->getSelectedSlot())
		{
			--slotTick;
			if(slotTick == 0 && !stop)
			{
				ItemInstance* itemStack = gui->minecraft.getLocalPlayer()->inventory->getItem(touchedSlot);
				if(!itemStack)
					return;

				int savedCount = itemStack->count - 1;
				itemStack->count = 1;
				gui->minecraft.getLocalPlayer()->drop(itemStack);
				itemStack->count = savedCount;
				
				if(itemStack->count == 0)
				{
					gui->minecraft.getLocalPlayer()->inventory->clearSlot(touchedSlot);
					stop = true;
				}

				slotTick = --delay;
			}
		}
	}
	else
	{
		slotTick = delay = 15;
		stop = false;
	}
}


JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	MSHookFunction((void*) &Gui::_tickItemDrop, (void*) &Gui$_tickItemDrop, (void**) &_Gui$_tickItemDrop);

	return JNI_VERSION_1_2;
}

