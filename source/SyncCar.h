class SyncCar;

#pragma once
#include "MainHeader.h"
#include "Packets.h"

class SyncCar{
	CVehicle* game_car_ptr;
	int32_t server_id;
	//CarInfo infoc;

public:
    SyncCar(int32_t server_id /* server id for this ped */);
	~SyncCar();
	CVehicle* getCar();
	//void setCarInfo(CarInfo& infoc);
};
