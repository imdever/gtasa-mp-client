#include "AbstractConnection.h"
using namespace std;


AbstractConnection::AbstractConnection() {

}

AbstractConnection::AbstractConnection(AbstractListener* listener_ptr, SOCKET& socket) {
    AbstractConnection::listener_ptr = listener_ptr;
    AbstractConnection::socket = socket;
    connected = true;
}

AbstractConnection::AbstractConnection(AbstractConnection* connection) {
    AbstractConnection::listener_ptr = connection->getListener();
    AbstractConnection::socket = connection->getSocket();
    connected = true;
    connection->socket = static_cast<SOCKET>(NULL);
}

AbstractConnection::~AbstractConnection() {
    if (socket != static_cast<SOCKET>(NULL))
        ::closesocket(socket);
}


AbstractConnection::AbstractConnection(AbstractConnection& connection) {
    socket = connection.getSocket();
    listener_ptr = connection.getListener();
    connected = true;
    connection.socket = static_cast<SOCKET>(NULL);
}


void AbstractConnection::startReading() {
    if (isReading())
        return;
    this->socketConnected();
    reading = true;
    connected = true;
    receive_thread_ptr = new thread([&, this]() {
        receiveHandler();
        });
    receive_thread_ptr->detach();
}

void AbstractConnection::receiveHandler() {
    while (isConnected()) {
        Packet receive_packet = receivePacket();
        if (receive_packet.packet_data == nullptr) {
            // something wrong with packet, maybe it disconnected?
            continue;
        }
        if (isConnected()) // maybe we got disconnected with this packet
            this->packetReceived(receive_packet);
    }

    while (delete_this == false) {
        std::this_thread::yield();
    }
}

Packet AbstractConnection::receivePacket() {
    PacketInfo packet_info;
    if (receive(&packet_info, sizeof(PacketInfo)) == false) {
        return Packet();
    }
    Packet packet;
    packet.info = packet_info;
    packet.packet_data = new char[packet_info.packet_size];
    if (receive(packet.packet_data, packet_info.packet_size) == false) {
        delete[] static_cast<char*>(packet.packet_data);
        return Packet();
    }
    return packet;
}

void AbstractConnection::sendPacket(void* data, int32_t size) {
    if (!isConnected()) {
        // socket is not connected, error!;
        return;
    }
    Packet send_packet;
    send_packet.info.packet_size = size;
    send(socket, reinterpret_cast<char*>(&send_packet.info), sizeof(PacketInfo), 0);
    send(socket, static_cast<char*>(data), size, 0);
}

bool AbstractConnection::receive(void* buffer, uint32_t size) {
    uint32_t received_bytes = 0;
    while (received_bytes < size) {
        uint32_t bytes = recv(socket, static_cast<char*>(buffer) + received_bytes, size - received_bytes, 0);
        if (bytes == SOCKET_ERROR) {
            // disconnect socket
            disconnect();
            return false;
        }
        received_bytes += bytes;
    }
    return true;
}


void AbstractConnection::packetReceived(Packet packet) {
    cout << "Packe received default handler.\n";
    delete[](char*)packet.packet_data;
}

bool AbstractConnection::isConnected() {
    return connected;
}

bool AbstractConnection::isReading() {
    return reading;
}

SOCKET AbstractConnection::getSocket() {
    return socket;
}

AbstractListener* AbstractConnection::getListener() {
    return listener_ptr;
}


void AbstractConnection::disconnect() {
    connected = false;
    this->socketDisconnected();
    if (listener_ptr != nullptr) {
        listener_ptr->dropConnection(socket);
    }
}

void AbstractConnection::socketDisconnected() {
    cout << "Socket disconnected, default handler.\n";
}

void AbstractConnection::socketConnected() {
    cout << "Socket connected, default handler.\n";
}

AbstractConnection* AbstractConnection::connect(string ip, uint16_t port) {
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        cout << "Invalid WinApi sockets version" << endl;
        while (true);
    }

    SOCKET client_socket;
    if ((client_socket = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cout << "Faild to create socket" << endl;
        while (true);
    }

    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);


    if (::connect(client_socket, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
        while (true);
    }

    return new AbstractConnection(0, client_socket);
}

void AbstractConnection::deleteThis() {
    delete_this = true;
}
