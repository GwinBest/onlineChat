#include "MessageBuffer.h"

namespace Buffer
{
	MessageBuffer::~MessageBuffer()
	{
		while (this->_head != nullptr)
		{
			MessageBuffer::popFront();
		}
	}

	MessageBuffer& MessageBuffer::getInstance()
	{
		static MessageBuffer instance;
		return instance;
	}
	
	void MessageBuffer::pushFront(const MessageType messageType, const char* data) noexcept
	{
		size_t dataLength = strlen(data);
		Node* temp = this->_head;

		if (this->_head == nullptr)
		{
			this->_head = new Node;
			
			this->_head->data = new char[dataLength + 1];
			strcpy_s(this->_head->data, dataLength + 1, data);

			this->_head->messageType = messageType;
			
			return;
		}
			
		while (temp->next != nullptr)
		{
			temp = temp->next;
		}

		Node* newNode = new Node;
		newNode->data = new char[dataLength + 1];
		strcpy_s(newNode->data, dataLength + 1, data);

		newNode->messageType = messageType;

		temp->next = newNode;
		temp->next->previous = temp;
	}

	void MessageBuffer::popFront() noexcept
	{
		if (this->_head->next == nullptr)
		{
			delete this->_head;
			this->_head = nullptr;
			return;
		}

		Node* temp = this->_head;
		while (temp->next != nullptr)
		{
			temp = temp->next;
		}
		
		temp->previous->next = nullptr;

		delete[] temp->data;
		delete temp;
	}

	inline bool MessageBuffer::isEmpty() const
	{
		return (this->_head == nullptr) ? true : false;
	}

}//!namespace Buffer
