#pragma once
#include "circular_buffer.h"

namespace expos {

	enum MESSAGETYPE {
		MESSAGE_DISPLAYEVENT,
	};

#define RECIEVER_GAME		0x01
#define RECIEVER_FILEMAN	0x02
#define RECIEVER_DISPLAY	0x04
#define RECIEVER_AUDIO		0x08
#define RECIEVER_LOGIC		0x10
#define RECIEVER_SCRIPT		0x20



	struct Message {
		union {
			struct {
				int val1, val2, val3, val4;
			} general;
		} data;
		MESSAGETYPE type;
		uint8_t recievers;
	};

	class MessageReciever {
	protected:
		uint8_t recieving;
	public:

		virtual void recieve(Message*) = 0;

		uint8_t getRecieving() { return recieving; }

	};

	class MessageBus {
	private:
		circular_buffer<Message> messages;
	public:
		MessageBus();
		~MessageBus();

		Message *createMessage();




	};


}