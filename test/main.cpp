#include "../src/client/client.h"
#include "../src/buffer.h"

using namespace Network;

int main()
{
	Client client("127.0.0.1", 1111);
	if(client.Connect())
		client.StartReceiveThread();
	

	

	std::string buffer;
	size_t id;
	while (true)
	{
		std::cin >> id;
		std::cin >> buffer;
		client.Send(id, buffer.c_str(), buffer.size());

	}

	system("pause");

	return 0;
}