#ifndef BSOCKET_H
#define BSOCKET_H


#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#endif

#define READ_BUF_SIZE 1024
#define SEND_BUF_SIZE 1024
#define ADDR_LIST_SIZE 16

#include <cstdlib>

#ifdef _WIN32
#define _WINSOCKAPI_
#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <ws2tcpip.h>
#else  
// assume not Windows is POSIX style sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <unistd.h> 
#endif

#include <iostream>
#include <cstring>

#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
typedef unsigned int SOCKET;

static SOCKET m_server_socket = INVALID_SOCKET;


class bsocket
{
public:
    bsocket();
    bsocket(std::string UDP_IP, int UDP_Port, bool listen);
    ~bsocket();

#ifdef _WIN32
    int SocketStartup(WSADATA* wsaData);
#endif
    SOCKET createSocket();
    int bindToSocket(SOCKET server_socket, struct sockaddr_in server_addr);
    int connectToSocket(SOCKET server_socket, struct sockaddr_in server_addr);
    void openSocket();
    void listenSocket();
    int closeSocket();
    int receiveMessage();
    int sendMessage(const char* packet);
    char* readBuffer() { return m_server_buf; }
    struct sockaddr_in* getSocket() { return m_si_other; }
    void setAddress(std::string address) { m_address = address; }
    void setPort(int port) { m_port = port; }
    void listenThread();

private:
    char* m_server_buf;
    std::string m_address;
    int m_port;
    struct sockaddr_in* m_si_other;
};

#endif //BSOCKET_H