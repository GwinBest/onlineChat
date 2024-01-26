#include "Server.h"
#include <variant>
void Server::Start() noexcept
{
	if (WSAStartup(_dllVersion, &_wsaData) != 0)
	{
		std::cout << "Error" << std::endl;
		exit(1);
	}

	_socketAddress.sin_addr.s_addr = inet_addr(_ipAddress.c_str());
	_socketAddress.sin_port = htons(_port);
	_socketAddress.sin_family = AF_INET;

	_serverSocket = socket(AF_INET, SOCK_STREAM, NULL);
	bind(_serverSocket, (SOCKADDR*)&_socketAddress, sizeof(_socketAddress));
	listen(_serverSocket, SOMAXCONN);
}

void Server::Run() noexcept
{
	int sizeOfServerAddress = sizeof(_socketAddress);
	SOCKET newConnection;
	for (int i = 0; i < 100; i++)
	{
		newConnection = accept(_serverSocket, (SOCKADDR*)&_socketAddress, &sizeOfServerAddress);

		if (newConnection == 0)
		{
			std::cout << "Error #2\n";
		}
		else
		{
			char clientIP[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(_socketAddress.sin_addr), clientIP, INET_ADDRSTRLEN);

			std::cout << "Client Connected! " << _connectionsCurrentCount << " " << clientIP << std::endl;

			_connections[i] = newConnection;
			_connectionsCurrentCount++;

			std::thread clientThread(&Server::ClientHandler, this, i);
			clientThread.detach();
		}
	}
}

void Server::ClientHandler(int index)
{
	std::string existedUserName;
	std::string response;
	bool boolResponce;

	char request[255];

	ActionType actionType;

	int32_t recvLength = 0;

	while (true)
	{
		if (recvLength = recv(_connections[index], (char*)&actionType, sizeof(actionType), NULL));
		{
			if (recvLength <= 0)
			{
				closesocket(_connections[index]);
				return;
			}

			switch (actionType)
			{
			case Server::ActionType::kSendChatMessage:
			{
				size_t currentUserLoginSize;
				char curentUserLogin[50];
				recv(_connections[index], (char*)&currentUserLoginSize, sizeof(currentUserLoginSize), NULL);
				recv(_connections[index], curentUserLogin, currentUserLoginSize, NULL);
				curentUserLogin[currentUserLoginSize] = '\0';

				size_t selectedUserLoginSize;
				char selectedUserLogin[50];
				recv(_connections[index], (char*)&selectedUserLoginSize, sizeof(selectedUserLoginSize), NULL);
				recv(_connections[index], selectedUserLogin, selectedUserLoginSize, NULL);
				selectedUserLogin[selectedUserLoginSize] = '\0';

				char message[4097];
				size_t messageSize;
				recv(_connections[index], (char*)&messageSize, sizeof(messageSize), NULL);
				recv(_connections[index], message, messageSize, NULL);
				message[messageSize] = '\0';

				try
				{
					sql::Driver* driver;
					sql::Connection* con;
					sql::Statement* stmt;
					sql::ResultSet* res = nullptr;

					driver = get_driver_instance();
					con = driver->connect("127.0.0.1:3306", "root", "admin");
					con->setSchema("onlinechat");

					snprintf(request, 255, "SELECT * FROM onlinechat.chats WHERE chatParticipants = '%s %s';", selectedUserLogin, curentUserLogin);
					stmt = con->createStatement();
					res = stmt->executeQuery(request);

					if (!res->next())
					{
						snprintf(request, 255, "SELECT * FROM onlinechat.chats WHERE chatParticipants = '%s %s';", curentUserLogin, selectedUserLogin);
						stmt = con->createStatement();
						res = stmt->executeQuery(request);
						if (!res->next())
						{
							snprintf(request, 255, "Insert into chats(chatParticipants, chatName) VALUES ('%s %s',' ');", curentUserLogin, selectedUserLogin);
							stmt = con->createStatement();
							stmt->executeUpdate(request);

						}
					}

					snprintf(request, 255, "SELECT * FROM onlinechat.chats WHERE chatParticipants = '%s %s';", selectedUserLogin, curentUserLogin);
					stmt = con->createStatement();
					res = stmt->executeQuery(request);
					if (!res->next())
					{
						snprintf(request, 255, "SELECT * FROM onlinechat.chats WHERE chatParticipants = '%s %s';", curentUserLogin, selectedUserLogin);
						stmt = con->createStatement();
						res = stmt->executeQuery(request);
						res->next();
					}

					size_t chatId = res->getInt("chatId");

					snprintf(request, 255, "INSERT INTO messages (chatId, author, msg) VALUES ('%zu', '%s', '%s');", chatId, curentUserLogin, message);

					stmt = con->createStatement();
					stmt->executeUpdate(request);

					for (size_t i = 0; i < _connectionsCurrentCount; ++i)
					{
						send(_connections[i], (char*)&actionType, sizeof(actionType), NULL);
						send(_connections[i], selectedUserLogin, sizeof(selectedUserLogin), NULL);
						send(_connections[i], message, 4096, NULL);
					}

					delete res;
					delete stmt;
					delete con;
				}
				catch (sql::SQLException& e)
				{
					std::cout << "error MySQL: " << e.what() << std::endl;
				}

				break;
			}
			case Server::ActionType::kAddUserCredentialsToDatabase:
			case Server::ActionType::kCheckUserExistence:
			case Server::ActionType::kGetUserNameFromDatabase:
			case Server::ActionType::kFindUsersByLogin:
			{
				size_t nameLength;
				char name[50];
				recv(_connections[index], (char*)&nameLength, sizeof(nameLength), NULL);
				recv(_connections[index], name, nameLength, NULL);
				name[nameLength] = '\0';

				size_t loginLength;
				char login[50];
				recv(_connections[index], (char*)&loginLength, sizeof(loginLength), NULL);
				recv(_connections[index], login, loginLength, NULL);
				login[loginLength] = '\0';

				size_t password = 0;
				recv(_connections[index], (char*)&password, sizeof(password), NULL);

				try
				{
					sql::Driver* driver;
					sql::Connection* con;
					sql::Statement* stmt = nullptr;
					sql::ResultSet* res = nullptr;

					driver = get_driver_instance();
					con = driver->connect("127.0.0.1:3306", "root", "admin");
					con->setSchema("onlinechat");

					switch (actionType)
					{
					case Server::ActionType::kAddUserCredentialsToDatabase:
					{
						snprintf(request, 255, "%s%s%s%s%s%zu%s", "INSERT INTO users (userName, userLogin, userPassword) VALUES ('", name, "', '", login, "', '", password, "');");
						stmt = con->createStatement();
						stmt->executeUpdate(request);

						std::cout << "OK" << std::endl;

						break;
					}
					case Server::ActionType::kCheckUserExistence:
					{
						//TODO: if login - "" - return
						snprintf(request, 255, "SELECT * FROM onlinechat.users WHERE userLogin = '%s' AND userPassword = '%zu';", login, password);
						stmt = con->createStatement();
						res = stmt->executeQuery(request);

						if (!res->next())
						{
							boolResponce = false;
						}
						else
						{
							existedUserName = res->getString("userName");
							boolResponce = true;
						}

						send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);

						send(_connections[index], (char*)&boolResponce, sizeof(bool), NULL);

						break;
					}
					case Server::ActionType::kGetUserNameFromDatabase:
					{
						response = existedUserName;
						size_t resposeSize = response.size();

						send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);

						send(_connections[index], (char*)&resposeSize, sizeof(resposeSize), NULL);
						send(_connections[index], response.c_str(), resposeSize, NULL);

						break;
					}
					case Server::ActionType::kFindUsersByLogin:
					{
						snprintf(request, 255, "SELECT * FROM onlinechat.users  where userLogin like '%s%%';", login);
						stmt = con->createStatement();
						res = stmt->executeQuery(request);

						size_t counter = 0;
						std::string foundUsersLogin[100];
						while (res->next())
						{
							int	id = res->getInt("id");
							foundUsersLogin[counter++] = res->getString("userLogin");
						}

						send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);
						send(_connections[index], (char*)&counter, sizeof(counter), NULL);

						int i = 0;
						while (counter > 0)
						{
							//TODO: do not send user login length
							size_t resultLength = foundUsersLogin[i].size();

							send(_connections[index], (char*)&resultLength, sizeof(resultLength), NULL);
							send(_connections[index], foundUsersLogin[i++].c_str(), resultLength, NULL);
							counter--;
						}
						break;
					}
					default:
						break;
					}

					delete res;
					delete stmt;
					delete con;
				}
				catch (sql::SQLException& e)
				{
					std::cout << "error MySQL: " << e.what() << std::endl;

					delete name;
					delete login;
				}

				break;
			}
			case Server::ActionType::kGetAvailableChatsForUser:
			case Server::ActionType::kReceiveAllMessagesForSelectedChat:
			{
				size_t loginSize;
				char login[50];
				recv(_connections[index], (char*)&loginSize, sizeof(loginSize), NULL);
				recv(_connections[index], login, loginSize, NULL);
				login[loginSize] = '\0';

				size_t chatId;
				recv(_connections[index], (char*)&chatId, sizeof(chatId), NULL);

				switch (actionType)
				{
				case Server::ActionType::kGetAvailableChatsForUser:
				{
					try
					{
						sql::Driver* driver;
						sql::Connection* con;
						sql::Statement* stmt;
						sql::ResultSet* res;

						driver = get_driver_instance();
						con = driver->connect("127.0.0.1:3306", "root", "admin");
						con->setSchema("onlinechat");

						snprintf(request, 255, "SELECT * FROM onlinechat.chats where chatParticipants like '%%%s%%' ;", login);
						stmt = con->createStatement();
						res = stmt->executeQuery(request);

						size_t counter = 0;
						std::string result[100];
						size_t resultInt[100];
						while (res->next())
						{
							resultInt[counter] = res->getInt("chatId");
							result[counter] = res->getString("chatParticipants");
							size_t pos = result[counter].find(login);
							if (pos != std::string::npos)
							{
								if (pos == 0)
								{
									result[counter].erase(pos, loginSize + 1);
								}
								else
								{
									result[counter].erase(pos - 1, loginSize + 1);

								}
							}
							counter++;
						}

						send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);
						send(_connections[index], (char*)&counter, sizeof(counter), NULL);

						int i = 0;
						while (counter > 0)
						{
							size_t resultLength = result[i].size();
							send(_connections[index], (char*)&resultInt[i], sizeof(size_t), NULL);

							send(_connections[index], (char*)&resultLength, sizeof(resultLength), NULL);
							send(_connections[index], result[i].c_str(), resultLength, NULL);
							counter--;
							i++;
						}

						delete res;
						delete stmt;
						delete con;

					}
					catch (sql::SQLException& e)
					{
						std::cout << "error MySQL: " << e.what() << std::endl;
					}

					break;
				}
				case Server::ActionType::kReceiveAllMessagesForSelectedChat:
				{

					try {
						sql::Driver* driver;
						sql::Connection* con;
						sql::Statement* stmt;
						sql::ResultSet* res;

						driver = get_driver_instance();
						con = driver->connect("127.0.0.1:3306", "root", "admin");
						con->setSchema("onlinechat");

						char request[255];
						snprintf(request, 255, "SELECT * FROM onlinechat.messages where chatId = '%zu';", chatId);
						stmt = con->createStatement();
						res = stmt->executeQuery(request);

						std::string msg[1000];
						uint8_t msgtype[1000];
						size_t count = 0;
						while (res->next())
						{
							std::string author = res->getString("author");
							msg[count] = res->getString("msg");
							if (!strcmp(login, author.c_str()))
							{
								msgtype[count] = 1;
							}
							else
							{
								msgtype[count] = 2;
							}
							count++;
						}

						send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);
						send(_connections[index], (char*)&count, sizeof(count), NULL);


						int i = 0;
						while (count > 0)
						{
							size_t msgSize = msg[i].size();
							send(_connections[index], (char*)&msgSize, sizeof(msgSize), NULL);
							send(_connections[index], msg[i].c_str(), msgSize, NULL);
							send(_connections[index], (char*)&msgtype[i++], sizeof(msgtype[i]), NULL);
							count--;
						}


						std::cout << "OK" << std::endl;

						delete res;
						delete stmt;
						delete con;

					}
					catch (sql::SQLException& e) {
						std::cout << "error MySQL: " << e.what() << std::endl;

					}

					break;
				}
				}

				break;
			}
			default:
				break;
			}
		}
	}
}
