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
#include <cstring>

int main()
{
#ifdef _WIN32
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0)
  {
    std::cerr << "❌ Error al crear el socket\n";
    return 1;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8081);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    std::cerr << "❌ Error al hacer bind\n";
    return 1;
  }

  if (listen(server_socket, 5) < 0)
  {
    std::cerr << "❌ Error en listen\n";
    return 1;
  }

  std::cout << "✅ Servidor escuchando en puerto 8081...\n";

  while (true)
  {
    sockaddr_in client_addr{};
#ifdef _WIN32
    int client_size = sizeof(client_addr);
#else
    socklen_t client_size = sizeof(client_addr);
#endif
    int client_socket = accept(server_socket, (sockaddr *)&client_addr, &client_size);
    if (client_socket < 0)
    {
      std::cerr << "❌ Error al aceptar cliente\n";
      continue;
    }

    char buffer[1024] = {0};
#ifdef _WIN32
    int bytes = recv(client_socket, buffer, sizeof(buffer), 0);
#else
    ssize_t bytes = recv(client_socket, buffer, sizeof(buffer), 0);
#endif

    if (bytes > 0)
    {
      std::cout << "📨 Cliente dijo: " << buffer << "\n";

      std::string respuesta = "Hola cliente, recibí: ";
      respuesta += buffer;
#ifdef _WIN32
      send(client_socket, respuesta.c_str(), static_cast<int>(respuesta.size()), 0);
#else
      send(client_socket, respuesta.c_str(), respuesta.size(), 0);
#endif
    }
  }

#ifdef _WIN32
  WSACleanup();
#endif

  return 0;
}
