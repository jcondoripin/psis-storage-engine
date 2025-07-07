#include "../src/network/client.hpp"
#include <iostream>

int main()
{
    try
    {
        AsyncClient client;

        client.setOnMessage([](const std::string &msg)
                            {
            std::cout << "\n[Server]: " << msg << "\n> ";
            std::cout.flush(); });

        client.connectToServer();

        std::string line;
        while (true)
        {
            std::cout << "> ";
            if (!std::getline(std::cin, line))
                break;
            if (line == "exit")
                break;
            client.sendMessage(line);
        }

        client.disconnect();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
    }

    return 0;
}
