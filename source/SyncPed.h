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
	//CarInfo infoc;

public:
    //float ped_rot_delta;
	SyncPed(int32_t server_id /* server id for this ped */);
	~SyncPed();

	CPed* getPed();
	PedInfo getPedInfo();
	void setPedInfo(PedInfo& info);
	//void setCarInfo(CarInfo& infoc);
};
