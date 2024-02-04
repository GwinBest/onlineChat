#include "../src/server/server.h"
#include "../src/database/database.h"

int main()
{
	sql::ResultSet* res =  Database::DatabaseHelper::GetInstance().ExecuteQuery("dasd");

	if (res->next())
	{
		return 1;
	}
	ServerNetworking::Server server;
	server.Start();
	server.Run();

	return 0;
}