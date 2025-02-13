#include <iostream>

#include "server/server.h"

int main()
{
    ServerNetworking::Server server;
    if (!server.Start())
    {
        std::cout << "Cant start the server\n";
        return 1;
    }

    server.Run();

    return 0;
}