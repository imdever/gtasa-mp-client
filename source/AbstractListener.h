class AbstractListener;
struct ListenerParams;

#pragma once
#include <iostream>
#include <winsock2.h>
#include <winsock.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include "AbstractConnection.h"


struct ListenerParams {
    uint16_t port = 0;   // if zero struct isn't initialized
    char host_name[256]; // server bind hostname
    struct hostent* hp;  // server bind ip
};

class AbstractListener {
protected:
    ListenerParams params;
    SOCKADDR_IN addr;
    SOCKET listen_socket;

    std::list<AbstractConnection*> connections_list;
    uint32_t connections_count = 0;
    bool listening = false;

public:
    AbstractListener();
    virtual ~AbstractListener();

    // Set new listener parameters
    virtual void setListenerParams(ListenerParams params);

    // Start listenin using by ListenerParams class variable
    virtual bool startListening();

    // Functions for handle some server events
    virtual void newConnection(SOCKET connection_socket);
    virtual void dropConnection(SOCKET droped_socket);
    virtual void listenerHandle();
    const std::list<AbstractConnection*>& getConnections();

    uint16_t getConnectionCount();
    bool isListening();
};
