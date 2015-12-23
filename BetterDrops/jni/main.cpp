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

struct DropDelayTimer {
	int8_t _delay;
	int NEXTDELAY;
	bool STOP;
	
	DropDelayTimer() {
		reset();
	}
	
	const DropDelayTimer& operator--() {
		--_delay;
			
		return *this;
	}
	
	void reset() {
		_delay = NEXTDELAY = 15;
		STOP = false;
	}
	
	void proceed() {
		if(NEXTDELAY > 1)
			_delay = --NEXTDELAY;
	}
	
	void stop() {
		STOP = true;
	}
	
	operator bool() const {
		return _delay > 0 || STOP;
	}
};

static DropDelayTimer slotTick;
static bool stop = false;

void (*_Gui$_tickItemDrop)(Gui*);
void Gui$_tickItemDrop(Gui* gui) {
	if(MenuPointer::isPressed())
	{
		int touchedSlot = gui->getSlotIdAt(MenuPointer::getX(), MenuPointer::getY());
		if(touchedSlot == gui->minecraft.getLocalPlayer()->inventory->getSelectedSlot())
		{
			--slotTick;
			if(!slotTick)
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
					slotTick.stop();
				}

				slotTick.proceed();
			}
		}
	}
	else
	{
		slotTick.reset();
	}
}


JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	MSHookFunction((void*) &Gui::_tickItemDrop, (void*) &Gui$_tickItemDrop, (void**) &_Gui$_tickItemDrop);

	return JNI_VERSION_1_2;
}

