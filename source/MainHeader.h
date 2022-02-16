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
#endif
#include "CHud.h"
using namespace std;

#include "SyncPed.h"
#include "PlayerClient.h"
#pragma comment(lib, "ws2_32.lib")

extern int pedModelIds[];
using namespace plugin;


class MultiplayerPlugin {
    HANDLE handle;
    PlayerClient* player = nullptr;
public:
    MultiplayerPlugin();
    static CPed* spawnPed();
    static void output(string s);
};