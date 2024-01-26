#include "../src/server/server.h"

int main(int argc, char* argv[])
{
	Server server;
	server.Start();
	server.Run();

	return 0;
}