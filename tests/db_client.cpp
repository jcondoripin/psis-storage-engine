// client.cpp
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <iostream>
#include <string>

int main()
{
#ifdef _WIN32
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    std::cerr << "❌ Error creando el socket\n";
    return 1;
  }

  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8081);
  inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

  if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
  {
    std::cerr << "❌ Error conectando al servidor\n";
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    return 1;
  }

  std::cout << "✅ Conectado al servidor. Escribe un comando:\n";

  std::string input;
  while (true)
  {
    std::cout << "> ";
    std::getline(std::cin, input);
    if (input == "exit")
      break;

    send(sock, input.c_str(), input.size(), 0);

    char buffer[1024] = {};
    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0)
    {
      buffer[bytes] = '\0';
      std::cout << "🧾 Respuesta: " << buffer << "\n";
    }
  }

#ifdef _WIN32
  closesocket(sock);
  WSACleanup();
#else
  close(sock);
#endif

  return 0;
}
