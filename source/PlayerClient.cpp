#include "PlayerClient.h"


PlayerClient::PlayerClient() {
    memset(&sync_peds, 0x00, sizeof(sync_peds));
}

PlayerClient::PlayerClient(AbstractConnection connection) : AbstractConnection(connection) {
    for (int32_t i = 0; i < 1024; i++) {
        sync_peds[i] = nullptr;
    }
}

void PlayerClient::socketConnected() {
    MultiplayerPacket packet;
    packet.type = PacketType::REQUEST_ID;
    sendPacket(&packet, sizeof(MultiplayerPacket));
}

void PlayerClient::packetReceived(Packet packet) {
    MultiplayerPacket* multiplayer_packet = reinterpret_cast<MultiplayerPacket*>(packet.packet_data);
    pedSyncHandler();
    lockAcess();
    if (packet.packet_data == nullptr)
        return;
    if ( multiplayer_packet->type == PacketType::YOU_ID) {
        YourIDPacket* your_id_packet_ptr = static_cast<YourIDPacket*>(multiplayer_packet);
        client_id = your_id_packet_ptr->user_id;
        MultiplayerPlugin::output("Player ID [" + std::to_string(client_id) + "]\n\r");
    }
    if( multiplayer_packet->type == PacketType::CREATE_PED ){
        CreatePedPacket* create_packet_ptr = static_cast<CreatePedPacket*>(multiplayer_packet);
        if (sync_peds[create_packet_ptr->ped_id] == nullptr){
            sync_peds[create_packet_ptr->ped_id] = new SyncPed(create_packet_ptr->ped_id);
        } else {
            MultiplayerPlugin::output("CREATE NOT FREE PED\n\r");
        }
        MultiplayerPlugin::output("Spawn PED of player  [" + std::to_string(create_packet_ptr->ped_id) + "]\n\r");
    }
    if (multiplayer_packet->type == PacketType::DELETE_PED) {
        DeletePedPacket* delete_packet_ptr = static_cast<DeletePedPacket*>(multiplayer_packet);
        if( sync_peds[delete_packet_ptr->ped_id] != nullptr )
            delete sync_peds[delete_packet_ptr->ped_id];
        else 
            MultiplayerPlugin::output("DELETE FREE PED\n\r");
        sync_peds[delete_packet_ptr->ped_id] = nullptr;
        MultiplayerPlugin::output("Delete PED of player [" + std::to_string(delete_packet_ptr->ped_id) + "]\n\r");
    }
    if (multiplayer_packet->type == PacketType::UPDATE_PED) {
        UpdatePedPacket* update_packet_ptr = static_cast<UpdatePedPacket*>(multiplayer_packet);
        if(sync_peds[update_packet_ptr->ped_id] != nullptr ){
            sync_peds[update_packet_ptr->ped_id]->setPedInfo(update_packet_ptr->info);
        } else {
            MultiplayerPlugin::output("UPDATE NULL PED\n\r");
        }
    }
    // delete packet data
    delete[] static_cast<char*>(packet.packet_data);
    unlockAcess();
}

void PlayerClient::pedSyncHandler() {
    lockAcess();
    UpdatePedPacket update_packet;
    update_packet.type   = PacketType::UPDATE_PED;
    update_packet.ped_id = this->client_id;
    update_packet.info   = PlayerClient::getPlayerPedInfo();
    sendPacket(&update_packet, sizeof(UpdatePedPacket));
    unlockAcess();
}

PedInfo PlayerClient::getPlayerPedInfo() {
    PedInfo info;
    CVector pos = FindPlayerPed()->GetPosition();
    info.pos.x = pos.x;
    info.pos.y = pos.y;
    info.pos.z = pos.z;
    return info;
}

void PlayerClient::deleteLater() {
    delete_later = true;
}


void PlayerClient::lockAcess() {
    access_mutex.lock();
}

void PlayerClient::unlockAcess() {
    access_mutex.unlock();
}