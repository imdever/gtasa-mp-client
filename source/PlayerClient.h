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
#include "MainHeader.h"
#include "Packets.h"
#include "AbstractConnection.h"
#include "AbstractListener.h"
#include <chrono>
#include <thread>

class PlayerClient : public AbstractConnection {
    int32_t client_id;
    SyncPed* sync_peds[1500]; // 1000 for players 500 for another peds
    std::thread* player_sync_thread_ptr;
    bool delete_later = false;

public:
    PlayerClient();
    PlayerClient(AbstractConnection connection);
    PedInfo getPlayerPedInfo();
    void pedSyncHandler();
    void deleteLater();

protected:
    void socketConnected() override;
    void packetReceived(Packet packet);
};
