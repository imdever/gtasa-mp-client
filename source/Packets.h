#pragma once
#include <stdint.h>

#pragma pack(push, 1)
enum class PacketType {
    REQUEST_ID, FAIL_TO_REQUEST_ID, YOU_ID,
    CREATEM_PLAYER, DELETE_PLAYER,
    CREATE_PED, DELETE_PED, UPDATE_PED
};

struct Vector3d {
    double x, y, z;
};

struct PedInfo {
    Vector3d pos;
};

struct MultiplayerPacket {
    PacketType type;
};

struct YourIDPacket : public MultiplayerPacket {
    int32_t user_id;
};

struct CreatePedPacket : public MultiplayerPacket {
    int32_t ped_id;
};

struct UpdatePedPacket : public MultiplayerPacket {
    int32_t ped_id;
    PedInfo info;
};

struct DeletePedPacket : public MultiplayerPacket {
    int32_t ped_id;
};
#pragma pack(pop)
