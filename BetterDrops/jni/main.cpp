#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <stdlib.h>
#include <string>

#include "substrate.h"


typedef void Mouse;

typedef void Level;
typedef struct {
     char filler[104]; // 0
     Level* level; // 104
} MinecraftClient;

typedef struct {
     char filler[2616]; // 0
     MinecraftClient& mc; // 2616
} Gui;

typedef struct {
	int count; // 0
	int data; // 4
	void* item; // 8
} ItemInstance;

typedef struct {
	char filler[40]; // 0
	int selectedslot; // 40
} Inventory;

typedef struct {
	char filler[3212]; // 0
	Inventory* inv; // 3212
} Player;


static Mouse* Mouse$_instance;
static Player* player_inst;
static Inventory* inventory_inst;
static Level* level_inst;

int tick = 15;
int nextTick = 15;
bool canContinue = true;


static int (*Mouse$getX)(Mouse*);
static int (*Mouse$getY)(Mouse*);
static bool (*Mouse$isButtonDown)(Mouse*, int);
static int (*Gui$getSlotIdAt)(Gui*, int, int);
static void (*Player$drop)(Player*, ItemInstance*);
//static void (*Level$playSound)(Level*, Player*, std::string const&, float, float);
static void (*FillingContainer$replaceSlot)(void*, int, ItemInstance*);
static void (*FillingContainer$clearSlot)(void*, int);
static ItemInstance* (*FillingContainer$getItem)(void*, int);

static void (*_Gui$tickItemDrop)(Gui*);
static void (*_Player$Player)(Player*, void*, bool);


void Gui$tickItemDrop(Gui* gui) {
    if(player_inst == NULL) return; // Minecraft, why you tick item drop before a player is initialized???
    if(!Mouse$isButtonDown(Mouse$_instance, 1)) { // If we are not touching the screen
        nextTick = 15;
        tick = nextTick;
        canContinue = true;
    }
	inventory_inst = player_inst->inv;
	level_inst = gui->mc.level;

	int x = Mouse$getX(Mouse$_instance);
	int y = Mouse$getY(Mouse$_instance);
	int id = Gui$getSlotIdAt(gui, x, y);
	if(id == player_inst->inv->selectedslot) { // If we are holding the slot we have selected; TODO: redundant?
		if(tick == 0 && canContinue) { // Once the tick loop is done, and we're allowed to continue
			ItemInstance* item = FillingContainer$getItem((void*) inventory_inst, id); // Get the item we're holding
            int save = item->count; // The next part has to be done in a specific order, so we store the current count
            item->count = 1;
            Player$drop(player_inst, item); // Drop one instance of the current item
            //Level$playSound(level_inst, player_inst, "random.pop", 1.0, 0.3);
            // Play a sound; Disabled because it keeps crashing
            
            item->count = save - 1;
            if(item->count == 0) {
                 FillingContainer$clearSlot((void*) inventory_inst, id);
                 canContinue = false;
            }
            else FillingContainer$replaceSlot((void*) inventory_inst, id, item);

            if(nextTick > 0) nextTick--; // Used to increase drop speed after each loop
            tick = nextTick; // Reset the loop
        } else tick--; // If we're still in the tick loop, substract the tick by one
	}
}

void Player$Player(Player* player, void* level, bool hi) {
	player_inst = player; // Used to get our Player instance
	_Player$Player(player, level, hi);
}


JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	void* handle = dlopen("libminecraftpe.so", RTLD_LAZY);

	Mouse$getX = (int (*)(Mouse*)) dlsym(handle, "_ZN11MouseDevice4getXEv");
	Mouse$getY = (int (*)(Mouse*)) dlsym(handle, "_ZN11MouseDevice4getYEv");
    Mouse$isButtonDown = (bool (*)(Mouse*, int)) dlsym(handle, "_ZN11MouseDevice12isButtonDownEi");
    Mouse$_instance = (Mouse*) dlsym(handle, "_ZN5Mouse9_instanceE");
	Gui$getSlotIdAt = (int (*)(Gui*, int, int)) dlsym(handle, "_ZN3Gui11getSlotIdAtEii");
	Player$drop = (void (*)(Player*, ItemInstance*)) dlsym(handle, "_ZN6Player4dropEPK12ItemInstance");
	//Level$playSound = (void (*)(Level*, Player*, std::string const&, float, float)) dlsym(handle, "_ZN5Level9playSoundEP5EntityRKSsff");
	FillingContainer$replaceSlot = (void (*)(void*, int, ItemInstance*)) dlsym(handle, "_ZN16FillingContainer11replaceSlotEiP12ItemInstance");
    FillingContainer$clearSlot = (void (*)(void*, int)) dlsym(handle, "_ZN16FillingContainer9clearSlotEi");
  	FillingContainer$getItem = (ItemInstance* (*)(void*, int)) dlsym(handle, "_ZN16FillingContainer7getItemEi");

	void* gui$tickItemDrop = dlsym(handle, "_ZN3Gui12tickItemDropEv");
	MSHookFunction(gui$tickItemDrop, (void*) &Gui$tickItemDrop, (void**) &_Gui$tickItemDrop);

	void* player$Player = dlsym(handle, "_ZN6PlayerC2ER5Levelb");
	MSHookFunction(player$Player, (void*) &Player$Player, (void**) &_Player$Player);

	return JNI_VERSION_1_2;
}
