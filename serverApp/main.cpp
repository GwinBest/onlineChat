#include "../src/server/server.h"

int main()
{
	ServerNetworking::Server server;
	server.Start();
	server.Run();

	return 0;
}