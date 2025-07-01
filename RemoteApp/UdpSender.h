#pragma once
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
// #include <windows.h> // 필요할 때만, 그리고 반드시 winsock2.h 이후에!
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <vector>
#include <string>

class UdpSender {
public:
    UdpSender(const std::string& ip, int port);
    ~UdpSender();
    bool send(const std::vector<uint8_t>& data);
private:
#ifdef _WIN32
    SOCKET sock;
    WSADATA wsaData;
#else
    int sock;
#endif
    struct sockaddr_in addr;
};
