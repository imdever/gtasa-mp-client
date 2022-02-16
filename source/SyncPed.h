class SyncPed;
struct PedInfo;
struct Vector3d;

#pragma once
#include "MainHeader.h"
#include "Packets.h"


class SyncPed{
	CPed* game_ped_ptr;
	int32_t server_id;
	PedInfo info;
	
public:
	SyncPed(int32_t server_id /* server id for this ped */);
	~SyncPed();

	void setPedInfo(PedInfo& info);
};
