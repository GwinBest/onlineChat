#include <iostream>

#include "../src/server/server.h"

int main()
{
    ServerNetworking::Server server;
    if (!server.Start())
    {
        std::cout << "Cant start the server\n";
        return 0;
    }

    server.Run();

    return 0;
}