#include "SyncPed.h"


SyncPed::SyncPed(int32_t server_id /* server id for this ped */) {
	SyncPed::server_id = server_id;
	game_ped_ptr = MultiplayerPlugin::spawnPed();
}

SyncPed::~SyncPed() {
	delete game_ped_ptr;
}


void SyncPed::setPedInfo(PedInfo& info) {
	SyncPed::info = info;
	static CVector pos;
	static float rott;
	pos.x = info.pos.x;
	pos.y = info.pos.y;
	pos.z = info.pos.z;
	rott = info.currot;
}
/*
void SyncPed::setCarInfo(CarInfo& infoc) {
	SyncPed::infoc = infoc;
	static CVector pos;
	pos.x = infoc.pos.x;
	pos.y = infoc.pos.y;
	pos.z = infoc.pos.z;
}*/

CPed* SyncPed::getPed() {
	if( game_ped_ptr->IsPointerValid())
		return game_ped_ptr;
	return nullptr;
}


PedInfo SyncPed::getPedInfo() {
	return info;
}
