#include "../src/client/client.h"

using namespace Network;

int main()
{
	Client::getInstance().Send(1,"af",1);
	Client::getInstance().Send(1,"gdg",1);

	//std::thread thread(&Client::Receive, &client);

	//std::string buffer;
	//size_t id;
	//while (true)
	//{
	//	std::cin >> id;
	//	std::cin >> buffer;
	//	client.Send(id, buffer.c_str(), buffer.size());

	//}

	//system("pause");

	//thread.join();
	return 0;
}