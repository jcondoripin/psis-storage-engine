#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <atomic>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
using socklen_t = int;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
using SOCKET = int;
#endif

class Server
{
public:
  using Handler = std::function<std::string(const std::string &)>;

  Server(const std::string &port,
         const std::string &host,
         size_t bufferSize,
         int backlog = SOMAXCONN)
      : port_(port), host_(host), bufferSize_(bufferSize), backlog_(backlog), listenSock_(INVALID_SOCKET), running_(false) {}

  ~Server()
  {
    stop();
  }

  void setHandler(Handler h)
  {
    handler_ = std::move(h);
  }

  bool start()
  {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
      return false;
#endif
    struct addrinfo hints{}, *result = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    const char *node = host_.empty() ? nullptr : host_.c_str();

    if (getaddrinfo(node, port_.c_str(), &hints, &result) != 0)
      return false;

    listenSock_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSock_ == INVALID_SOCKET)
    {
      freeaddrinfo(result);
      return false;
    }

    if (bind(listenSock_, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
    {
      freeaddrinfo(result);
      closeSocket(listenSock_);
      return false;
    }

    freeaddrinfo(result);

    if (listen(listenSock_, backlog_) == SOCKET_ERROR)
    {
      closeSocket(listenSock_);
      return false;
    }

    running_ = true;
    acceptThread_ = std::thread([this]
                                { acceptLoop(); });
    return true;
  }

  void stop()
  {
    if (!running_)
      return;
    running_ = false;
    closeSocket(listenSock_);
    if (acceptThread_.joinable())
      acceptThread_.join();
#ifdef _WIN32
    WSACleanup();
#endif
  }

private:
  void acceptLoop()
  {
    while (running_)
    {
      SOCKET client = accept(listenSock_, nullptr, nullptr);
      if (client == INVALID_SOCKET)
      {
        if (running_)
          continue;
        else
          break;
      }
      std::thread(&Server::clientLoop, this, client).detach();
    }
  }

  void clientLoop(SOCKET client)
  {
    std::vector<char> buf(bufferSize_);
    while (running_)
    {
      int recvd = recv(client, buf.data(), (int)buf.size() - 1, 0);
      if (recvd > 0)
      {
        buf[recvd] = '\0';
        std::string req(buf.data());
        std::string resp = handler_ ? handler_(req) : std::string();
        send(client, resp.c_str(), (int)resp.size(), 0);
      }
      else
      {
        break;
      }
    }
    closeSocket(client);
  }

  static void closeSocket(SOCKET s)
  {
#ifdef _WIN32
    closesocket(s);
#else
    ::close(s);
#endif
  }

  std::string port_;
  std::string host_;
  size_t bufferSize_;
  int backlog_;
  SOCKET listenSock_;
  std::atomic<bool> running_;
  std::thread acceptThread_;
  Handler handler_;
};
