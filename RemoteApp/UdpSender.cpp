#include "UdpSender.h"

UdpSender::UdpSender(const std::string& ip, int port) {
#ifdef _WIN32
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sock = socket(AF_INET, SOCK_DGRAM, 0);
#else
    sock = socket(AF_INET, SOCK_DGRAM, 0);
#endif
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

UdpSender::~UdpSender() {
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
}

bool UdpSender::send(const std::vector<uint8_t>& data) {
    int len = static_cast<int>(data.size());
    int sent = sendto(sock, reinterpret_cast<const char*>(data.data()), len, 0, (struct sockaddr*)&addr, sizeof(addr));
    return sent == len;
}
