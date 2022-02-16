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
	pos.x = info.pos.x;
	pos.y = info.pos.y;
	pos.z = info.pos.z;
	game_ped_ptr->SetPosn(pos);
	game_ped_ptr->UpdatePosition();
}