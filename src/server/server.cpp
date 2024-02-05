#include "Server.h"

#include <WS2tcpip.h>

#include <iostream>
#include <string>
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

		for (int i = 0; i < sizeof(_connections) / sizeof(_connections[0]); i++)
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
			recvReturnValue = recv(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);

			if (recvReturnValue <= 0)
			{
				closesocket(_connections[index]);
				index--;
				
				std::cout << GetLastError() << std::endl;
				std::cout << "Client dissconnected: " << index << std::endl;

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

				size_t senderUserId;
				recv(_connections[index], reinterpret_cast<char*>(&senderUserId), sizeof(senderUserId), NULL);

				size_t receiverUserLoginSize;
				char receiverUserLogin[Common::userLoginSize];
				recv(_connections[index], reinterpret_cast<char*>(&receiverUserLoginSize), sizeof(receiverUserLoginSize), NULL);
				recv(_connections[index], receiverUserLogin, receiverUserLoginSize, NULL);
				receiverUserLogin[receiverUserLoginSize] = '\0';

				size_t receiverUserId;
				recv(_connections[index], reinterpret_cast<char*>(&receiverUserId), sizeof(receiverUserId), NULL);

				size_t messageSize;
				char message[Common::maxInputBufferSize];	
				recv(_connections[index], reinterpret_cast<char*>(&messageSize), sizeof(messageSize), NULL);
				recv(_connections[index], message, messageSize, NULL);
				message[messageSize] = '\0';

				try
				{
					resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
						"SELECT DISTINCT r1.chatId "
						"FROM relations r1 "
						"JOIN relations r2 ON r1.chatId = r2.chatId "
						"WHERE r1.userId = %zu AND r2.userId = %zu;",
						senderUserId, receiverUserId);

					size_t chatId = 0;

					if (!resultSet->next())
					{
						Database::DatabaseHelper::GetInstance().ExecuteUpdate(
							"INSERT INTO chats(chatParticipants, chatName) "
							"VALUES('%d %d', ' ');",
							senderUserId, receiverUserId);

						resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
							"SELECT LAST_INSERT_ID();");

						if (resultSet->next())
						{
							chatId = resultSet->getInt("last_insert_id()");
						}

						Database::DatabaseHelper::GetInstance().ExecuteUpdate(
							"INSERT INTO relations(userId, chatId) "
							"VALUES "
							"(%d,%d),"
							"(%d,%d);",
							senderUserId, chatId,
							receiverUserId, chatId);

						Database::DatabaseHelper::GetInstance().ExecuteUpdate(
							"INSERT INTO user_chat_names(user_id, chat_id, chat_name) "
							"VALUES "
							"(%d,%d,'%s'),"
							"(%d,%d,'%s');",
							senderUserId, chatId, receiverUserLogin,
							receiverUserId, chatId, senderUserLogin);
					}
					else
					{
						chatId = resultSet->getInt("chatId");
					}

					Database::DatabaseHelper::GetInstance().ExecuteUpdate(
						"INSERT INTO messages (chatId, author, msg) "
						"VALUES (%zu, '%zu', '%s');",
						chatId, senderUserId, message);

					for (size_t i = 0; i < _connectionsCurrentCount; ++i)
					{
						send(_connections[i], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);

						send(_connections[i], reinterpret_cast<char*>(&receiverUserId), sizeof(receiverUserId), NULL);

						size_t messageSize = strlen(message);
						send(_connections[i], reinterpret_cast<char*>(&messageSize), sizeof(messageSize), NULL);
						send(_connections[i], message, messageSize, NULL);
					}
				}
				catch (const sql::SQLException& e)
				{
					SendServerErrorMessage(index, "Server error: server cant push message to database");

					break;
				}

				break;
			}
			case NetworkCore::ActionType::kAddUserCredentialsToDatabase:
			{
				NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(index);

				if (userPacket.name.empty())
				{
					SendServerErrorMessage(index, "Server error: user name is empty");

					break;
				}

				if (userPacket.login.empty())
				{
					SendServerErrorMessage(index, "Server error: user login is empty");

					break;
				}

				if (userPacket.password == 0)
				{
					SendServerErrorMessage(index, "Server error: user password is empty");

					break;
				}

				bool result = false;

				try
				{
					result = Database::DatabaseHelper::GetInstance().ExecuteUpdate(
						"INSERT INTO users(userName, userLogin, userPassword) "
						"VALUES('%s', '%s', %zu);",
						userPacket.name.c_str(), userPacket.login.c_str(), userPacket.password);

					send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);
					send(_connections[index], reinterpret_cast<char*>(&result), sizeof(result), NULL);
				}
				catch (const sql::SQLException& e)
				{
					send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);
					send(_connections[index], reinterpret_cast<char*>(&result), sizeof(result), NULL);

					SendServerErrorMessage(index, "Server error: server cant add user credentials to database");
				}

				break;
			}
			case NetworkCore::ActionType::kCheckUserExistence:
			{
				NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(index);

				if (userPacket.name.empty())
				{
					SendServerErrorMessage(index, "Server error: user login is empty");

					break;
				}

				if (userPacket.login.empty())
				{
					SendServerErrorMessage(index, "Server error: user login is empty");

					break;
				}

				if (userPacket.password == 0)
				{
					SendServerErrorMessage(index, "Server error: user password is empty");

					break;
				}

				if (userPacket.id == 0)
				{
					SendServerErrorMessage(index, "Server error: user password is empty");

					break;
				}

				try
				{
					resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
						"SELECT * FROM users "
						"WHERE userName = '%s' AND userLogin = '%s' "
						"AND userPassword = %zu AND id = %zu;",
						userPacket.name.c_str(), userPacket.login.c_str(),
						userPacket.password, userPacket.id);

					bool result = false;

					if (resultSet->next())
					{
						result = true;
					}

					send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);
					send(_connections[index], reinterpret_cast<char*>(&result), sizeof(result), NULL);
				}
				catch (const sql::SQLException& e)
				{
					SendServerErrorMessage(index, "Server error: server cant check user existence");
				}

				break;
			}
			case NetworkCore::ActionType::kGetUserNameFromDatabase:
			{
				NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(index);

				if (userPacket.login.empty())
				{
					SendServerErrorMessage(index, "Server error: user login is empty");

					break;
				}

				try
				{
					resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
						"SELECT userName FROM users "
						"WHERE userLogin = '%s';",
						userPacket.login.c_str());

					std::string result = "";
					size_t resposeSize = result.size();

					if (resultSet->next())
					{
						result = resultSet->getString("userName");
					}

					resposeSize = result.size();

					send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);

					send(_connections[index], reinterpret_cast<char*>(&resposeSize), sizeof(resposeSize), NULL);
					send(_connections[index], result.c_str(), resposeSize, NULL);
				}
				catch (const sql::SQLException& e)
				{
					SendServerErrorMessage(index, "Server error: server cant get user name from database");
				}

				break;
			}
			case NetworkCore::ActionType::kGetUserIdFromDatabase:
			{
				NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(index);

				if (userPacket.login.empty())
				{
					SendServerErrorMessage(index, "Server error: user login is empty");

					break;
				}

				try
				{
					resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
						"SELECT id FROM users "
						"WHERE userLogin = '%s';",
						userPacket.login.c_str());

					size_t result = 0;

					if (resultSet->next())
					{
						result = resultSet->getInt("id");
					}

					send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);

					send(_connections[index], reinterpret_cast<char*>(&result), sizeof(result), NULL);
				}
				catch (const sql::SQLException& e)
				{
					SendServerErrorMessage(index, "Server error: server cant get user name from database");
				}

				break;
			}
			case NetworkCore::ActionType::kFindUsersByLogin:
			{
				NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(index);

				std::string* foundUsersLogin = nullptr;

				try
				{
					resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
						"SELECT userLogin FROM users "
						"WHERE userLogin like '%s%%';",
						userPacket.login.c_str());

					size_t counter = 0;
					foundUsersLogin = new std::string[resultSet->rowsCount()];

					while (resultSet->next())
					{
						foundUsersLogin[counter++] = resultSet->getString("userLogin");
					}

					send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);
					send(_connections[index], reinterpret_cast<char*>(&counter), sizeof(counter), NULL);

					int i = 0;
					while (counter > 0)
					{
						size_t resultLength = foundUsersLogin[i].size();

						send(_connections[index], reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
						send(_connections[index], foundUsersLogin[i++].c_str(), resultLength, NULL);

						counter--;
					}
				}
				catch (const sql::SQLException& e)
				{
					SendServerErrorMessage(index, "Server error: server cant push message to database");
				}

				delete[] foundUsersLogin;

				break;
			}
			case NetworkCore::ActionType::kGetAvailableChatsForUser:
			{
				NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(index);

				if (userPacket.id == 0)
				{
					SendServerErrorMessage(index, "Server error: user id is empty");

					break;
				}

				std::string* chatNameResult = nullptr;
				size_t* chatIdResult = nullptr;

				try
				{
					resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
						"SELECT chat_id, chat_name "
						"FROM user_chat_names "
						"WHERE user_id = %zu;",
						userPacket.id);

					size_t counter = 0;
					chatNameResult = new std::string[resultSet->rowsCount()];
					chatIdResult = new size_t[resultSet->rowsCount()];

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
				}
				catch (const sql::SQLException& e)
				{
					SendServerErrorMessage(index, "Server error: server cant get available chats for user");
				}

				delete[] chatNameResult;
				delete[] chatIdResult;

				break;
			}
			case NetworkCore::ActionType::kReceiveAllMessagesForSelectedChat:
			{
				size_t userLoginSize;
				char userLogin[Common::userLoginSize];
				recv(_connections[index], reinterpret_cast<char*>(&userLoginSize), sizeof(userLoginSize), NULL);
				recv(_connections[index], userLogin, userLoginSize, NULL);
				userLogin[userLoginSize] = '\0';

				size_t chatId;
				recv(_connections[index], reinterpret_cast<char*>(&chatId), sizeof(chatId), NULL);

				if (userLogin[0] == '\0')
				{
					SendServerErrorMessage(index, "Server error: user login is empty");

					break;
				}

				if (chatId == 0)
				{
					SendServerErrorMessage(index, "Server error: chat id is empty");

					break;
				}

				std::string* messagesResult = nullptr;
				MessageBuffer::MessageStatus* messageTypeResult = nullptr;

				try
				{
					resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
						"SELECT * "
						"FROM messages "
						"WHERE chatId = %zu;",
						chatId);

					messagesResult = new std::string[resultSet->rowsCount()];
					messageTypeResult = new MessageBuffer::MessageStatus[resultSet->rowsCount()];
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
				}
				catch (const sql::SQLException& e)
				{
					SendServerErrorMessage(index, "Server error: server cant receive all messages for selected chat");
				}

				delete[] messagesResult;
				delete[] messageTypeResult;

				break;
			}
			default:
				break;
			}
		}
	}

	NetworkCore::UserPacket Server::ReceiveUserCredentialsPacket(size_t index) const noexcept
	{
		NetworkCore::UserPacket newUserPacket;

		size_t userNameLength;
		char userName[Common::userNameSize] = "";
		recv(_connections[index], reinterpret_cast<char*>(&userNameLength), sizeof(userNameLength), NULL);
		recv(_connections[index], userName, userNameLength, NULL);
		userName[userNameLength] = '\0';
		newUserPacket.name = userName;

		size_t userLoginLength;
		char userLogin[Common::userLoginSize] = "";
		recv(_connections[index], reinterpret_cast<char*>(&userLoginLength), sizeof(userLoginLength), NULL);
		recv(_connections[index], userLogin, userLoginLength, NULL);
		userLogin[userLoginLength] = '\0';
		newUserPacket.login = userLogin;

		size_t userPassword = 0;
		recv(_connections[index], reinterpret_cast<char*>(&userPassword), sizeof(userPassword), NULL);
		newUserPacket.password = userPassword;

		size_t userId = 0;
		recv(_connections[index], reinterpret_cast<char*>(&userId), sizeof(userId), NULL);
		newUserPacket.id = userId;

		return newUserPacket;
	}

	void Server::SendServerErrorMessage(const size_t index, const std::string& errorMessage) const noexcept
	{
		NetworkCore::ActionType actionType = NetworkCore::ActionType::kServerError;

		send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);

		size_t erorrMessageSize = errorMessage.size();

		send(_connections[index], reinterpret_cast<char*>(&erorrMessageSize), sizeof(erorrMessageSize), NULL);
		send(_connections[index], errorMessage.c_str(), erorrMessageSize, NULL);
	}

} // !namespace ServerNetworking