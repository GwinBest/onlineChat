#include "Server.h"

#include <WS2tcpip.h>

#include <iostream>
#include <thread>

#include "../common/common.h"
#include "../database/database.h"
#include "../messageBuffer/messageBuffer.h"

namespace ServerNetworking
{
	void Server::Start() noexcept
	{
		if (WSAStartup(NetworkCore::dllVersion, &_wsaData) != 0)
		{
			std::cout << "Error" << std::endl;
			exit(SOCKET_ERROR);
		}

		_socketAddress.sin_addr.s_addr = inet_addr(NetworkCore::ipAddress.c_str());
		_socketAddress.sin_port = htons(NetworkCore::port);
		_socketAddress.sin_family = AF_INET;

		_serverSocket = socket(AF_INET, SOCK_STREAM, NULL);
		bind(_serverSocket, (SOCKADDR*)&_socketAddress, sizeof(_socketAddress));
		listen(_serverSocket, SOMAXCONN);
	}

	void Server::Run() noexcept
	{
		int sizeOfServerAddress = sizeof(_socketAddress);
		SOCKET newConnection;

		for (int i = 0; i < sizeof(_connections)/sizeof(_connections[0]); i++)
		{
			newConnection = accept(_serverSocket, reinterpret_cast<SOCKADDR*>(&_socketAddress), &sizeOfServerAddress);

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
		NetworkCore::ActionType actionType;
		int32_t recvReturnValue = 0;
		sql::ResultSet* resultSet = nullptr;

		while (true)
		{
			if (recvReturnValue = recv(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL))
			{
				if (recvReturnValue <= 0)
				{
					closesocket(_connections[index]);
					index--;

					return;
				}

				switch (actionType)
				{
				case NetworkCore::ActionType::kSendChatMessage:
				{
					size_t senderUserLoginSize;
					char senderUserLogin[Common::userLoginSize];
					recv(_connections[index], reinterpret_cast<char*>(&senderUserLoginSize), sizeof(senderUserLoginSize), NULL);
					recv(_connections[index], senderUserLogin, senderUserLoginSize, NULL);
					senderUserLogin[senderUserLoginSize] = '\0';

					size_t reciverUserLoginSize;
					char reciverUserLogin[Common::userLoginSize];
					recv(_connections[index], reinterpret_cast<char*>(&reciverUserLoginSize), sizeof(reciverUserLoginSize), NULL);
					recv(_connections[index], reciverUserLogin, reciverUserLoginSize, NULL);
					reciverUserLogin[reciverUserLoginSize] = '\0';

					size_t messageSize;
					char message[Common::maxInputBufferSize];
					recv(_connections[index], reinterpret_cast<char*>(&messageSize), sizeof(messageSize), NULL);
					recv(_connections[index], message, messageSize, NULL);
					message[messageSize] = '\0';

					resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
						"SELECT DISTINCT r1.chatId"
						"FROM relations r1"
						"JOIN relations r2 ON r1.chatId = r2.chatId"
						"WHERE r1.userId = 1 AND r2.userId = 2;");

					if (!resultSet->next())
					{
						Database::DatabaseHelper::GetInstance().ExecuteUpdate(
							"INSERT INTO chats(chatParticipants, chatName)"
							"VALUES('%s %s', ' ');",
							senderUserLogin, reciverUserLogin);

						Database::DatabaseHelper::GetInstance().ExecuteUpdate(
							"INSERT INTO relations(userId, chatId)"
							"VALUES(%d,%d);", 1, 2);
					}
					//TODO
					size_t chatId = resultSet->getInt("chatId");

					Database::DatabaseHelper::GetInstance().ExecuteUpdate(
						"INSERT INTO messages (chatId, author, msg)"
						"VALUES (%zu, '%s', '%s');",
						chatId, senderUserLogin, message);

					for (size_t i = 0; i < _connectionsCurrentCount; ++i)
					{
						send(_connections[i], (char*)&actionType, sizeof(actionType), NULL);
						send(_connections[i], senderUserLogin, sizeof(senderUserLogin), NULL);
						send(_connections[i], message, 4096, NULL);
					}

					break;
				}
				case NetworkCore::ActionType::kAddUserCredentialsToDatabase:
				case NetworkCore::ActionType::kCheckUserExistence:
				case NetworkCore::ActionType::kGetUserNameFromDatabase:
				case NetworkCore::ActionType::kFindUsersByLogin:
				{
					size_t userNameLength;
					char userName[Common::userNameSize] = "";
					recv(_connections[index], reinterpret_cast<char*>(&userNameLength), sizeof(userNameLength), NULL);
					recv(_connections[index], userName, userNameLength, NULL);
					userName[userNameLength] = '\0';

					size_t userLoginLength;
					char userLogin[Common::userLoginSize] = "";
					recv(_connections[index], reinterpret_cast<char*>(&userLoginLength), sizeof(userLoginLength), NULL);
					recv(_connections[index], userLogin, userLoginLength, NULL);
					userLogin[userLoginLength] = '\0';

					size_t userPassword = 0;
					recv(_connections[index], reinterpret_cast<char*>(&userPassword), sizeof(userPassword), NULL);

					switch (actionType)
					{
					case NetworkCore::ActionType::kAddUserCredentialsToDatabase:
					{
						bool result = false;

						if (userName[0] == '\0')
						{
							std::cout << "Error to add user credentials to database - userName is empty" << std::endl;

							send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);
							send(_connections[index], (char*)&result, sizeof(result), NULL);

							break;
						}

						if (userLogin[0] == '\0')
						{
							std::cout << "Error to add user credentials to database - userLogin is empty" << std::endl;

							send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);
							send(_connections[index], (char*)&result, sizeof(result), NULL);

							break;
						}

						if (userPassword == 0)
						{
							std::cout << "Error to add user credentials to database - password is empty" << std::endl;

							send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);
							send(_connections[index], (char*)&result, sizeof(result), NULL);

							break;
						}

						result = Database::DatabaseHelper::GetInstance().ExecuteUpdate(
							"INSERT INTO users(userName, userLogin, userPassword)"
							"VALUES('%s', '%s', %zu);",
							userName, userLogin, userPassword);

						send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);
						send(_connections[index], (char*)&result, sizeof(result), NULL);

						break;
					}
					case NetworkCore::ActionType::kCheckUserExistence:
					{
						bool result = false;

						if (userLogin[0] == '\0')
						{
							std::cout << "Error to check user existence - userLogin is empty" << std::endl;

							send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);
							send(_connections[index], (char*)&result, sizeof(result), NULL);

							break;
						}

						if (userPassword == 0)
						{
							std::cout << "Error to check user existence - password is empty" << std::endl;

							send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);
							send(_connections[index], (char*)&result, sizeof(result), NULL);

							break;
						}

						resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
							"SELECT * FROM users"
							"WHERE userLogin = '%s' AND userPassword = %zu ;",
							userLogin, userPassword);

						if (resultSet->next())
						{
							result = true;
						}

						send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);
						send(_connections[index], (char*)&result, sizeof(result), NULL);

						break;
					}
					case NetworkCore::ActionType::kGetUserNameFromDatabase:
					{
						std::string result = "";
						size_t resposeSize = result.size();

						if (userLogin[0] == '\0')
						{
							std::cout << "Error to get user name - userLogin is empty" << std::endl;

							send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);

							send(_connections[index], (char*)&resposeSize, sizeof(resposeSize), NULL);
							send(_connections[index], result.c_str(), resposeSize, NULL);

							break;
						}

						resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
							"SELECT userName FROM users"
							"WHERE userLogin = '%s';",
							userLogin);

						if (resultSet->next())
						{
							result = resultSet->getString("userName");
						}

						resposeSize = result.size();

						send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);

						send(_connections[index], (char*)&resposeSize, sizeof(resposeSize), NULL);
						send(_connections[index], result.c_str(), resposeSize, NULL);

						break;
					}
					case NetworkCore::ActionType::kFindUsersByLogin:
					{
						std::string result = "";
						size_t resposeSize = result.size();

						if (userLogin[0] == '\0')
						{
							std::cout << "Error to get user name - userLogin is empty" << std::endl;

							send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);

							send(_connections[index], (char*)&resposeSize, sizeof(resposeSize), NULL);
							send(_connections[index], result.c_str(), resposeSize, NULL);

							break;
						}

						resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
							"SELECT userLogin FROM users"
							"WHERE userLogin like '%s%%';",
							userLogin);

						if (resultSet == nullptr)
						{

						}


						size_t counter = 0;
						std::string* foundUsersLogin = new std::string[resultSet->rowsCount()];

						while (resultSet->next())
						{
							foundUsersLogin[counter++] = resultSet->getString("userLogin");
						}

						send(_connections[index], (char*)&actionType, sizeof(actionType), NULL);
						send(_connections[index], (char*)&counter, sizeof(counter), NULL);

						int i = 0;
						while (counter > 0)
						{
							size_t resultLength = foundUsersLogin[i].size();

							send(_connections[index], (char*)&resultLength, sizeof(resultLength), NULL);
							send(_connections[index], foundUsersLogin[i++].c_str(), resultLength, NULL);

							counter--;
						}

						delete[] foundUsersLogin;
						break;
					}
					default:
						break;
					}

					break;
				}
				case NetworkCore::ActionType::kGetAvailableChatsForUser:
				case NetworkCore::ActionType::kReceiveAllMessagesForSelectedChat:
				{
					size_t userLoginSize;
					char userLogin[Common::userLoginSize];
					recv(_connections[index], reinterpret_cast<char*>(&userLoginSize), sizeof(userLoginSize), NULL);
					recv(_connections[index], userLogin, userLoginSize, NULL);
					userLogin[userLoginSize] = '\0';

					size_t userId;

					size_t chatId;
					recv(_connections[index], (char*)&chatId, sizeof(chatId), NULL);

					switch (actionType)
					{
					case NetworkCore::ActionType::kGetAvailableChatsForUser:
					{
						resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
							"SELECT chat_id, chat_name "
							"FROM user_chat_names "
							"WHERE user_id = %zu;",
							userId);

						if (resultSet == nullptr)
						{

						}

						size_t counter = 0;
						std::string* chatNameResult = new std::string[resultSet->rowsCount()];
						size_t* chatIdResult = new size_t[resultSet->rowsCount()];

						while (resultSet->next())
						{
							chatNameResult[counter] = resultSet->getString("chat_name");
							chatIdResult[counter] = resultSet->getInt("chat_id");

							counter++;
						}

						send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);
						send(_connections[index], reinterpret_cast<char*>(&counter), sizeof(counter), NULL);

						int i = 0;
						while (counter > 0)
						{
							send(_connections[index], reinterpret_cast<char*>(&chatIdResult[i]), sizeof(chatIdResult[0]), NULL);

							size_t resultLength = chatNameResult[i].size();
							send(_connections[index], reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
							send(_connections[index], chatNameResult[i].c_str(), resultLength, NULL);

							counter--;
							i++;
						}

						delete[] chatNameResult;
						delete[] chatIdResult;

						break;
					}
					case NetworkCore::ActionType::kReceiveAllMessagesForSelectedChat:
					{
						resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
							"SELECT * "
							"FROM messages "
							"WHERE chatId = %zu;",
							chatId);

						if (resultSet == nullptr)
						{

						}

						std::string* messagesResult = new std::string[resultSet->rowsCount()];
						MessageBuffer::MessageStatus* messageTypeResult = new MessageBuffer::MessageStatus[resultSet->rowsCount()];
						size_t count = 0;

						while (resultSet->next())
						{
							std::string author = resultSet->getString("author");

							messagesResult[count] = resultSet->getString("msg");

							if (!strcmp(userLogin, author.c_str()))
							{
								messageTypeResult[count] = MessageBuffer::MessageStatus::kSend;
							}
							else
							{
								messageTypeResult[count] = MessageBuffer::MessageStatus::kReceived;
							}

							count++;
						}

						send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);
						send(_connections[index], reinterpret_cast<char*>(&count), sizeof(count), NULL);


						int i = 0;
						while (count > 0)
						{
							size_t msgSize = messagesResult[i].size();
							send(_connections[index], reinterpret_cast<char*>(&msgSize), sizeof(msgSize), NULL);
							send(_connections[index], messagesResult[i].c_str(), msgSize, NULL);
							send(_connections[index], reinterpret_cast<char*>(&messageTypeResult[i++]), sizeof(messageTypeResult[i]), NULL);
							count--;
						}

						delete[] messagesResult;
						delete[] messageTypeResult;

						break;
					}
					default:
						break;
					}

					break;
				}
				}
			}
		}
	}

} // !namespace ServerNetworking