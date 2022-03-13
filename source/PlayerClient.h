class PlayerClient;

#pragma once
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include <iostream>
#include <string>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "SyncPed.h"
#include "SyncCar.h"
#include "MainHeader.h"
#include "Packets.h"
#include "AbstractConnection.h"
#include "AbstractListener.h"
#include <chrono>
#include <mutex>
#include <thread>


class PlayerClient : public AbstractConnection {
    int32_t client_id;
    SyncPed* sync_peds[1500]; // 1000 for players 500 for another peds
    SyncCar* sync_cars[1000];
    std::mutex access_mutex;
    std::thread* ped_sync_thread_ptr;
    std::thread* car_thread_ptr;
    bool delete_later = false;
    //int peddid;
    //float ped_rotation_delta;


public:
    PlayerClient(AbstractConnection connection);
    PedInfo getPlayerPedInfo();
   // void GetRott();
    void playerSyncHandler();
    void updatePedsHandler();
    //void updateCarsHandler();
    void carSyncHandler();
    void SetCarDriver();
    void deleteLater();

    void lockAccess();
    void unlockAccess();

protected:
    void socketConnected() override;
    void packetReceived(Packet packet);
};
