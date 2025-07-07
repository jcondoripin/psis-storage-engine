// server_win.cpp
#define _WIN32_WINNT 0x0601
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#pragma comment(lib, "Ws2_32.lib")

constexpr char PORT[] = "8081";
constexpr size_t MAXBUF = 1024;

std::string handleClientMessage(const std::string &msg)
{
  return "Processed: " + msg;
}

int main()
{
  WSADATA wsaData;
  SOCKET listenSock = INVALID_SOCKET;
  SOCKET clientSock = INVALID_SOCKET;
  struct addrinfo hints{}, *result = nullptr;

  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
  {
    std::cerr << "WSAStartup failed\n";
    return 1;
  }

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(nullptr, PORT, &hints, &result) != 0)
  {
    std::cerr << "getaddrinfo failed\n";
    WSACleanup();
    return 1;
  }

  listenSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (listenSock == INVALID_SOCKET)
  {
    std::cerr << "socket creation failed: " << WSAGetLastError() << "\n";
    freeaddrinfo(result);
    WSACleanup();
    return 1;
  }

  if (bind(listenSock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
  {
    std::cerr << "bind failed: " << WSAGetLastError() << "\n";
    freeaddrinfo(result);
    closesocket(listenSock);
    WSACleanup();
    return 1;
  }
  freeaddrinfo(result);

  if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR)
  {
    std::cerr << "listen failed: " << WSAGetLastError() << "\n";
    closesocket(listenSock);
    WSACleanup();
    return 1;
  }
  std::cout << "Server listening on port " << PORT << "...\n";

  clientSock = accept(listenSock, nullptr, nullptr);
  if (clientSock == INVALID_SOCKET)
  {
    std::cerr << "accept failed: " << WSAGetLastError() << "\n";
    closesocket(listenSock);
    WSACleanup();
    return 1;
  }
  std::cout << "Client connected.\n";

  std::vector<char> recvBuf(MAXBUF);
  while (true)
  {
    int received = recv(clientSock, recvBuf.data(), (int)recvBuf.size() - 1, 0);
    if (received > 0)
    {
      recvBuf[received] = '\0';
      std::string msg(recvBuf.data());
      std::cout << "From client: " << msg << "\n";

      std::string response = handleClientMessage(msg);
      int sent = send(clientSock, response.c_str(), (int)response.size(), 0);
      if (sent == SOCKET_ERROR)
      {
        std::cerr << "send failed: " << WSAGetLastError() << "\n";
        break;
      }
    }
    else if (received == 0)
    {
      std::cout << "Client disconnected.\n";
      break;
    }
    else
    {
      std::cerr << "recv failed: " << WSAGetLastError() << "\n";
      break;
    }
  }

  closesocket(clientSock);
  closesocket(listenSock);
  WSACleanup();
  return 0;
}
