class AbstractConnection;
struct PacketInfo;
struct Packet;

#pragma once
#include <iostream>
#include <string>
#include <winsock2.h>
#include <winsock.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <mutex>
#include "AbstractListener.h"


#pragma pack(push, 1)
struct PacketInfo {
    int32_t packet_size;
};

struct Packet {
    PacketInfo info;
    void* packet_data;
};
#pragma pack(pop)

class AbstractConnection {
    class SocketDisconnected {
        int a;
    };
protected:
    SOCKET socket;
    AbstractListener* listener_ptr = nullptr; // null for client socket
    std::thread* receive_thread_ptr;
    std::mutex send_mutex;
    bool connected = false, delete_this = false, reading = false;

public:
    AbstractConnection();
    AbstractConnection(AbstractListener* listener, SOCKET& socket);
    AbstractConnection(AbstractConnection& connection); // MOVE connection
    AbstractConnection(AbstractConnection* connection); // MOVE connection
    static AbstractConnection* connect(std::string ip, uint16_t port);
    virtual ~AbstractConnection();

    bool isConnected();
    bool isReading();
    SOCKET getSocket();
    AbstractListener* getListener();

    virtual void sendPacket(void* data, int32_t size);
    void startReading();
    void deleteThis();

protected:
    /*
        Override this for complete packet receiving
        After use packet you need\can free packet memory by free [](char*)(packet_data)
    */
    virtual void packetReceived(Packet packet);
    /*
        Override this for reacting to socket disconnecting
    */
    virtual void socketDisconnected();
    /*
        Override this for reacting to socket connecting
    */
    virtual void socketConnected();

protected:
    // receive until size reached
    virtual bool receive(void* buffer, uint32_t size);
    virtual void receiveHandler();
    virtual Packet receivePacket();
    virtual void disconnect();
};
