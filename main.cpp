#include "src/core/database.hpp"
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <cstring>

volatile bool running = true;

int main(int argc, char *argv[])
{
  std::string host = "127.0.0.1";
  uint16_t port = 65535;
  std::string directory = "database/";

  for (int i = 1; i < argc; ++i)
  {
    if (std::strcmp(argv[i], "-h") == 0 && i + 1 < argc)
    {
      host = argv[++i];
    }
    else if (std::strcmp(argv[i], "-p") == 0 && i + 1 < argc)
    {
      port = static_cast<uint16_t>(std::stoi(argv[++i]));
    }
    else if (std::strcmp(argv[i], "--dir") == 0 && i + 1 < argc)
    {
      directory = argv[++i];
    }
    else
    {
      std::cerr << "Uso: " << argv[0] << " [-h <host>] [-p <port>] [--dir <directory>]\n";
      return 1;
    }
  }

  std::cout << "[Config]: Directorio -> " << directory << "\n";
  std::cout << "[Config]: Host -> " << host << "\n";
  std::cout << "[Config]: Puerto -> " << port << "\n";

  DatabaseServer dbs(directory, std::to_string(port), host);

  if (!dbs.start())
  {
    std::cerr << "❌ No se pudo iniciar el servidor.\n";
    return 1;
  }
  std::string input;
  while (input != "exit")
  {
    std::cout << "> ";
    if (!std::getline(std::cin, input))
    {
      break;
    }
    if (input == "exit")
    {
      std::cout << "[Server]: Comando 'exit' recibido. Deteniendo...\n";
      break;
    }
  }

  dbs.stop();
  std::cout << "[Server]: Servidor detenido.\n";

  return 0;
}
