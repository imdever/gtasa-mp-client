#include "SyncCar.h"

SyncCar::SyncCar(int32_t server_id /* server id for this ped */) {
	SyncCar::server_id = server_id;
	game_car_ptr = MultiplayerPlugin::SpawnCar();
}

SyncCar::~SyncCar() {
	delete game_car_ptr;
}

CVehicle* SyncCar::getCar()
{
    return game_car_ptr;
}
