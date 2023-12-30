#pragma once

#include <cstdint>
#include <list>
#include <string>
#include <memory>

namespace MessageBuffer
{

	enum class MessageStatus : uint8_t
	{
		kUndefined		= 0,
		kSend			= 1,
		kReceived		= 2
	};

	struct MessageNode
	{
		explicit MessageNode(MessageStatus messageType, std::string data) : _data(data), _messageType(messageType) {};
		
		std::string _data;
		MessageStatus _messageType = MessageStatus::kUndefined;
	};

	extern std::list<MessageNode> messageBuffer;

} // !namespace MessageBuffer

