#include "MessageBuffer.h"

namespace Buffer
{
	MessageBuffer::~MessageBuffer()
	{
		while (this->_head != nullptr)
		{
			MessageBuffer::PopFront();
		}
	}

	MessageBuffer& MessageBuffer::GetInstance()
	{
		static MessageBuffer instance;
		return instance;
	}
	
	void MessageBuffer::PushFront(const MessageType messageType, const char* data) noexcept
	{
		size_t dataLength = strlen(data);
		Node* temp = this->_head;

		if (this->_head == nullptr)
		{
			this->_head = new Node;
			
			this->_head->_data = new char[dataLength + 1];
			strcpy_s(this->_head->_data, dataLength + 1, data);

			this->_head->_messageType = messageType;
			
			return;
		}
			
		while (temp->_next != nullptr)
		{
			temp = temp->_next;
		}

		Node* newNode = new Node;
		newNode->_data = new char[dataLength + 1];
		strcpy_s(newNode->_data, dataLength + 1, data);

		newNode->_messageType = messageType;

		temp->_next = newNode;
		temp->_next->_previous = temp;
	}

	void MessageBuffer::PopFront() noexcept
	{
		if (this->_head->_next == nullptr)
		{
			delete this->_head;
			this->_head = nullptr;
			return;
		}

		Node* temp = this->_head;
		while (temp->_next != nullptr)
		{
			temp = temp->_next;
		}
		
		temp->_previous->_next = nullptr;

		delete[] temp->_data;
		delete temp;
	}

}//!namespace Buffer
