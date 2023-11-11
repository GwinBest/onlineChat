#ifndef _ONLINECHAT_BUFFER_H_
#define _ONLINECHAT_BUFFER_H_

#include <cstdint>
#include <string>

namespace MessageBuffer
{
	enum class MessageType : uint8_t
	{
		kUndefined = 0,
		kSend = 1,
		kReceived = 2
	};

	struct Node
	{
		std::string _data;
		MessageType _messageType = MessageType::kUndefined;

		Node* _next = nullptr;
		Node* _previous = nullptr;
	};

	class Iterator
	{
	public:
		Iterator(Node* first) : _current(first) {};

		inline Node& operator++() noexcept
		{
			_current = _current ? _current->_next : nullptr;
			return *(this->_current);
		}

		inline bool operator== (const Iterator& it) const noexcept
		{ 
			return this->_current == it._current;
		}
		inline bool operator!=(const Iterator& other) const noexcept 
		{ 
			return _current != other._current; 
		}
		
		inline Node& operator* ()const noexcept 
		{
			return *(this->_current); 
		}

	private:
		Node* _current;
	};

	class MessageBuffer
	{	
	public:
		using Iterator = Iterator;

		MessageBuffer() = default;
		~MessageBuffer();

		void PushFront(const MessageType messageType, const char* data) noexcept;
		void PopFront() noexcept;

		inline Iterator begin() const noexcept 
		{
			return Iterator(this->_head);
		}
		inline Iterator end() const noexcept 
		{ 
			return Iterator(nullptr); 
		}

	private:
		Node* _head = nullptr;
	};

}//!namespace Buffer

#endif // !_ONLINECHAT_BUFFER_H_
