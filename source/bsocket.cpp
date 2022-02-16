#include "bsocket.h"

using namespace std;


bsocket::bsocket() 
{
    m_si_other = new struct sockaddr_in;
}

bsocket::bsocket(string UDP_IP, int UDP_Port, bool listen) :
    m_server_buf(new char[READ_BUF_SIZE]),
    m_port(UDP_Port),
    m_address(UDP_IP)
{
    m_si_other = new struct sockaddr_in;
    if (listen)
        listenSocket();
    else
        openSocket();
}

#ifdef _WIN32
int bsocket::SocketStartup(WSADATA* wsaData)
{
    return WSAStartup(MAKEWORD(2, 2), wsaData);
}
#endif

SOCKET bsocket::createSocket()
{
#ifdef _WIN32
    SOCKET sockfd = INVALID_SOCKET;
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    return sockfd;
#else
    int sockfd = INVALID_SOCKET;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    return sockfd;
#endif
}

int bsocket::bindToSocket(SOCKET server_socket, struct sockaddr_in server_addr)
{
#ifdef _WIN32
    if (bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr))) {
        cout << "Bind failed with error " << WSAGetLastError() << endl;
        return EXIT_FAILURE;
    }
    return 0;
#else   
    if (bind(server_socket, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed with error");
        return EXIT_FAILURE;
    }
    return 0;
#endif 
}

int bsocket::connectToSocket(SOCKET server_socket, struct sockaddr_in server_addr)
{
#ifdef _WIN32
    if (connect(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr))) {
        std::cout << "Connect failed with error " << WSAGetLastError() << "\n";
        return EXIT_FAILURE;
    }
    return 0;
#else
    if (connect(server_socket, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connect failed with error");
        return EXIT_FAILURE;
    }
    return 0;
#endif
}

void bsocket::listenSocket()
{
    // setup address structure
    memset((char*)m_si_other, 0, sizeof(struct sockaddr_in));
    m_si_other->sin_family = AF_INET;
    m_si_other->sin_port = htons(m_port);
    
#ifdef _WIN32
    m_si_other->sin_addr.S_un.S_addr = inet_addr(m_address.c_str());
#else
    m_si_other->sin_addr.s_addr = inet_addr(m_address.c_str());
#endif

    // open socket connection
    if (m_server_socket == INVALID_SOCKET)
    {
#ifdef _WIN32
        // startup sockets
        WSADATA wsaData;
        if (m_wsComm->SocketStartup(&wsaData) == EXIT_FAILURE)
            exit(EXIT_FAILURE);
#endif

        // create socket
        m_server_socket = createSocket();
    }
}

void bsocket::openSocket()
{
    // setup address structure
    memset((char*)m_si_other, 0, sizeof(struct sockaddr_in));
    m_si_other->sin_family = AF_INET;
    m_si_other->sin_port = htons(m_port);
    
#ifdef _WIN32
    m_si_other->sin_addr.S_un.S_addr = inet_addr(m_address.c_str());
#else
    m_si_other->sin_addr.s_addr = inet_addr(m_address.c_str());
#endif

    // open socket connection
    if (m_server_socket == INVALID_SOCKET)
    {
#ifdef _WIN32
        // startup sockets
        WSADATA wsaData;
        if (m_wsComm->SocketStartup(&wsaData) == EXIT_FAILURE)
            exit(EXIT_FAILURE);
#endif

        // create socket
        m_server_socket = createSocket();

        if (bindToSocket(m_server_socket, *m_si_other) == EXIT_FAILURE)
        {
            perror("Could not bind to socket");
            exit(EXIT_FAILURE);
        }
    }
}

int bsocket::closeSocket()
{
#ifdef _WIN32
    if (shutdown(socket, SD_BOTH) == SOCKET_ERROR)
    {
        std::cout << "Socket shutdown failed: " << WSAGetLastError();
        closeSocket(m_server_socket);
        int res = WSACleanup();
        return res;
    }
    return 0;
#else
    int res = shutdown(m_server_socket, SHUT_RDWR);
    if (res == 0) { res = close(m_server_socket); }
    return 0;
#endif
}

int bsocket::receiveMessage()
{
    int bytes_received;
    int server_buf_len = READ_BUF_SIZE;

#ifdef _WIN32
    int sender_addr_size = sizeof(struct sockaddr_in);
    bytes_received = recvfrom(m_serverSocket, m_server_buf, server_buf_len, 0, (SOCKADDR*)m_si_other, &sender_addr_size);
    if (bytes_received == SOCKET_ERROR)
    {
        std::cout << "recvfrom failed with error " << WSAGetLastError() << "\n";
        return EXIT_FAILURE;
    }
#else
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
    bytes_received = recvfrom(m_server_socket, m_server_buf, server_buf_len, MSG_WAITALL, (struct sockaddr *)m_si_other, &peer_addr_len);
    if (bytes_received == SOCKET_ERROR)
    {
        perror("recvfrom failed with error");
        return EXIT_FAILURE;
    }
#endif

    m_server_buf[bytes_received] = '\0';

    return bytes_received;
}

int bsocket::sendMessage(const char* packet)
{
    int bytes_sent;
    int server_buf_len = READ_BUF_SIZE;

#ifdef _WIN32
    int sender_addr_size = sizeof(struct sockaddr_in);
    bytes_sent = sendto(m_server_socket, packet, strlen(packet), 0, (SOCKADDR*)m_si_other, sender_addr_size);
    if (bytes_sent == SOCKET_ERROR)
    {
        std::cout << "sendto failed with error " << WSAGetLastError() << "\n";
        return EXIT_FAILURE;
    }
#else
    size_t send_buf_len = strlen(packet);
    bytes_sent = sendto(m_server_socket, packet, send_buf_len, MSG_CONFIRM, (struct sockaddr*)m_si_other, sizeof(struct sockaddr));
    if (bytes_sent == SOCKET_ERROR)
    {
        perror("sendto failed with error");
        return EXIT_FAILURE;
    }
#endif
    
    return bytes_sent;
}