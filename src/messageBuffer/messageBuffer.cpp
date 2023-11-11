#include "MessageBuffer.h"

namespace Buffer
{
	MessageBuffer::~MessageBuffer()
	{
		while (this->_head->_next != nullptr)
		{
			MessageBuffer::PopFront();
		}

		delete this->_head;
		this->_head = nullptr;
	}
	
	void MessageBuffer::PushFront(const MessageType messageType, const char* data) noexcept
	{
		Node* temp = this->_head;

		if (this->_head == nullptr)
		{
			this->_head = new Node;
			
			this->_head->_data = data;
			this->_head->_messageType = messageType;
			
			return;
		}
			
		while (temp->_next != nullptr)
		{
			temp = temp->_next;
		}

		Node* newNode = new Node;
		newNode->_data = data;
		newNode->_messageType = messageType;

		temp->_next = newNode;
		temp->_next->_previous = temp;
	}

	void MessageBuffer::PopFront() noexcept
	{
		Node* temp = this->_head;
		while (temp->_next != nullptr)
		{
			temp = temp->_next;
		}
		
		temp->_previous->_next = nullptr;

		delete temp;
	}

}//!namespace Buffer
