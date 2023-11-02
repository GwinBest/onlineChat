#include "../src/client/client.h"

using namespace Network;

int main()
{
	Client client("127.0.0.1", 1111);
	client.Connect();

	std::thread thread(&Client::Receive, &client);

	std::string buffer;
	size_t id;
	while (true)
	{
		std::cin >> id;
		std::cin >> buffer;
		client.Send(id, buffer.c_str(), buffer.size());

	}

	system("pause");

	thread.join();
	return 0;
}