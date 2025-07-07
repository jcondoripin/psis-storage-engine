#ifdef _WIN32
    #define _WIN32_WINNT 0x0A00
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #include <windows.h>
    #include <process.h>  // Para _beginthreadex
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <fcntl.h>
    #include <sys/select.h>
#endif

#include <iostream>
#include <cstring>
#include <string>
#include <thread>
#include <atomic>

#ifdef _WIN32
// Variable global para el socket (simplificación)
SOCKET sock = INVALID_SOCKET;
std::atomic<bool> running(true);

unsigned __stdcall input_thread_func(void*) {
    char buffer[1024];
    while (running.load()) {
        if (fgets(buffer, sizeof(buffer), stdin) != nullptr) {
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';

            int sent = send(sock, buffer, (int)strlen(buffer), 0);
            if (sent == SOCKET_ERROR) {
                std::cerr << "Error enviando datos: " << WSAGetLastError() << "\n";
                running = false;
                break;
            }
        } else {
            running = false;  // EOF o error en stdin
            break;
        }
    }
    return 0;
}
#endif

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup falló\n";
        return 1;
    }
#endif

    const char* server_ip = "127.0.0.1";
    const int server_port = 8081;

#ifdef _WIN32
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Error creando socket: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }
#else
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error creando socket");
        return 1;
    }
#endif

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

#ifdef _WIN32
    wchar_t wserver_ip[INET_ADDRSTRLEN];
    size_t outSize;
    mbstowcs_s(&outSize, wserver_ip, server_ip, INET_ADDRSTRLEN);
    if (InetPtonW(AF_INET, wserver_ip, &server_addr.sin_addr) != 1) {
        std::cerr << "IP inválida o no soportada\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }
#else
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        std::cerr << "IP inválida o no soportada\n";
        close(sock);
        return 1;
    }
#endif

    while (true) {
#ifdef _WIN32
        if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            std::cerr << "Error conectando: " << WSAGetLastError() << ", reintentando en 3 segundos...\n";
            Sleep(3000);
        } else {
            break;
        }
#else
        if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Error conectando, reintentando en 3 segundos");
            sleep(3);
        } else {
            break;
        }
#endif
    }

    std::cout << "Conectado al servidor en " << server_ip << ":" << server_port << "\n";

#ifdef _WIN32
    // Crear hilo para lectura de consola y envío
    uintptr_t threadHandle = _beginthreadex(nullptr, 0, &input_thread_func, nullptr, 0, nullptr);
#endif

    char buffer[1024];
#ifdef _WIN32
    while (running.load()) {
#else
    while (true) {
#endif
        fd_set readfds;
        FD_ZERO(&readfds);
#ifdef _WIN32
        FD_SET(sock, &readfds);
        // select() en Windows necesita nfds = 0 cuando sólo monitoreas sockets
        int activity = select(0, &readfds, nullptr, nullptr, nullptr);
#else
        FD_SET(sock, &readfds);
        int activity = select(sock + 1, &readfds, nullptr, nullptr, nullptr);
#endif
        if (activity < 0) {
#ifdef _WIN32
            std::cerr << "Error en select(): " << WSAGetLastError() << "\n";
#else
            perror("Error en select()");
#endif
            break;
        }

        if (FD_ISSET(sock, &readfds)) {
#ifdef _WIN32
            int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
#else
            ssize_t bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
#endif
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';
                std::cout << "\nServidor dice: " << buffer << "\n> ";
                std::cout.flush();
            } else if (bytesReceived == 0) {
                std::cout << "Servidor cerró la conexión\n";
                break;
            } else {
#ifdef _WIN32
                std::cerr << "Error recibiendo datos: " << WSAGetLastError() << "\n";
#else
                perror("Error recibiendo datos");
#endif
                break;
            }
        }
    }

#ifdef _WIN32
    running = false;
    WaitForSingleObject((HANDLE)threadHandle, INFINITE);
    CloseHandle((HANDLE)threadHandle);
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif

    return 0;
}
