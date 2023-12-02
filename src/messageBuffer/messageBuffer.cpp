#include "MessageBuffer.h"

namespace Buffer
{
	MessageBuffer::~MessageBuffer()
	{
		while (_head != nullptr)
		{
			MessageBuffer::popFront();
		}
	}

	MessageBuffer& MessageBuffer::getInstance() noexcept
	{
		static MessageBuffer instance;
		return instance;
	}
	
	void MessageBuffer::pushFront(const MessageType messageType, const char* data) noexcept
	{
		size_t dataLength = strlen(data);
		Node* tempNode = _head;

		if (_head == nullptr)
		{
			_head = new Node;
			
			_head->data = new char[dataLength + 1];
			strcpy_s(_head->data, dataLength + 1, data);

			_head->messageType = messageType;
			
			return;
		}
			
		while (tempNode->next != nullptr)
		{
			tempNode = tempNode->next;
		}

		Node* newNode = new Node;
		newNode->data = new char[dataLength + 1];
		strcpy_s(newNode->data, dataLength + 1, data);

		newNode->messageType = messageType;

		tempNode->next = newNode;
		tempNode->next->previous = tempNode;
	}

	void MessageBuffer::popFront() noexcept
	{
		if (_head->next == nullptr)
		{
			delete _head;
			_head = nullptr;
			return;
		}

		Node* temp = _head;
		while (temp->next != nullptr)
		{
			temp = temp->next;
		}
		
		temp->previous->next = nullptr;

		delete[] temp->data;
		temp->data = nullptr;

		delete temp;
		temp = nullptr;
	}

	inline bool MessageBuffer::isEmpty() const noexcept
	{
		return (_head == nullptr) ? true : false;
	}

} // !namespace Buffer
