class MultiplayerPlugin;
#pragma once
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include "plugin.h"
#include "common.h"
#include "CTimer.h"
#include "CPopulation.h"
#include "CCivilianPed.h"
#include "CWorld.h"
#ifdef GTASA
#include "CStreaming.h"
#include "CTaskComplexWanderStandard.h"
#include "extensions/ScriptCommands.h"
#endif
#include "CHud.h"
#include "CMessages.h"
#include "CFont.h"
#include "CSprite.h"
#include "CModelInfo.h"
#include "CMonsterTruck.h"
#include "CQuadBike.h"
#include "CHeli.h"
#include "CPlane.h"
#include "CBmx.h"
#include "CTrailer.h"
#include "CBoat.h"
#include "CTheScripts.h"
using namespace std;

#include "SyncPed.h"
#include "PlayerClient.h"
#pragma comment(lib, "ws2_32.lib")

extern int pedModelIds[];
using namespace plugin;


class MultiplayerPlugin {
    HANDLE handle;
    PlayerClient* player = nullptr;
    //CPed* ped;
public:
    MultiplayerPlugin();
    static CPed* spawnPed();
    static void output(string s);
    static void SetMarkerPed(CPed *ped);//float x, float y, float z, int32_t playerid);
    static CVehicle *SpawnCar();
    //static void NickTags(CPed *ped);
};
