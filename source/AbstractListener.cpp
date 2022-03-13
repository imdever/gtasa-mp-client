#include "AbstractListener.h"
using namespace std;

AbstractListener::AbstractListener() {

}

AbstractListener::~AbstractListener() {

}

// Set new listener parameters
void AbstractListener::setListenerParams(ListenerParams params) {
    if (params.port == 0)
        throw string("port can't be zero");
    AbstractListener::params = params;
}

// Start listenin using by ListenerParams class variable
bool AbstractListener::startListening() {
    if (params.port == 0)
        throw string("port can't be zero");

    // Check Windows Sockets version
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        cout << DLLVersion;
        system("pause");
        return 0;
    }

    // Prepare parameters for listen socket
    addr.sin_family = AF_INET;
    addr.sin_port = htons(params.port);
    addr.sin_addr.s_addr = inet_addr(params.host_name);

    printf("Starting server. IP: %u.%u.%u.%u:%u\n", (unsigned char)addr.sin_addr.S_un.S_un_b.s_b1,
        (unsigned char)addr.sin_addr.S_un.S_un_b.s_b2,
        (unsigned char)addr.sin_addr.S_un.S_un_b.s_b3,
        (unsigned char)addr.sin_addr.S_un.S_un_b.s_b4, params.port);

    // Creating listening socket, and start listening
    listen_socket = socket(AF_INET, SOCK_STREAM, int(NULL));
    bind(listen_socket, (SOCKADDR*)&addr, sizeof(addr));
    listen(listen_socket, SOMAXCONN);

    // Create listening thread
    listening = true;
    listenerHandle();
}

// Functions for handle some server events
void AbstractListener::newConnection(SOCKET connection_socket) {
    AbstractConnection* new_connection = new AbstractConnection(this, connection_socket);
    connections_list.push_back(new_connection);
    connections_count++;
}

void AbstractListener::dropConnection(SOCKET droped_socket) {
    const std::lock_guard<std::mutex> lock(disconnect_mutex);
    AbstractConnection* delete_connection;
    std::list<AbstractConnection*>::iterator it;
    for (it = connections_list.begin(); it != connections_list.end(); ++it) {
        AbstractConnection* abstract_connection = *it;
        if (abstract_connection->getSocket() == droped_socket) {
            delete_connection = abstract_connection;
            connections_count--;
            break;
        }
    }
    connections_list.remove(delete_connection);
    delete_connection->deleteThis();
}

void AbstractListener::listenerHandle() {
    int sizeofaddr = sizeof(SOCKADDR_IN);
    SOCKET new_connection;
    while (isListening()) {
        new_connection = accept(listen_socket, (SOCKADDR*)&(addr), &sizeofaddr);
        if (new_connection == 0) {
            continue;
        }
        newConnection(new_connection);
    }
}

uint16_t AbstractListener::getConnectionCount() {
    return connections_count;
}

bool AbstractListener::isListening() {
    return listening;
}

const std::list<AbstractConnection*>& AbstractListener::getConnections() {
    return connections_list;
}
