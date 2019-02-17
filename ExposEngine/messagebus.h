#pragma once
#include "stdafx.h"
#include <allegro5/allegro.h>
#include "helper.h"

namespace expos {

	enum MESSAGETYPE {
		MESSAGE_DISPLAYEVENT,
		MESSAGE_WINDOW_LOADBITMAP
	};

constexpr auto RECIEVER_GAME = 0x01;
constexpr auto RECIEVER_FILEMAN = 0x02;
constexpr auto RECIEVER_DISPLAY = 0x04;
constexpr auto  RECIEVER_AUDIO = 0x08;
constexpr auto  RECIEVER_LOGIC = 0x10;
constexpr auto  RECIEVER_SCRIPT = 0x20;



	struct Message {
		union {
			struct {
				uint32_t val1, val2, val3, val4;
			} general;
			struct {
				ID_ref window, bitmap;
				MemfileInfo m;
			} window;
		} data;
		Message *next;
		MESSAGETYPE type;
		uint8_t recievers;
	};

	class MessageReciever {
	protected:
		uint8_t recieving;
	public:
		//returns true if the event was consumed and shouldn't be propagated further
		virtual bool recieve(Message*) = 0;

		uint8_t getRecieving() { return recieving; }

	};

	class MessageBus {
	private:
		ALLEGRO_MUTEX * mutexMQueue, *mutexRecievers;
		static constexpr size_t MAXMESSAGECOUNT = 64;
		Message * messages;

		Message *first = nullptr, *open = nullptr;
		std::vector<MessageReciever*> messageRecievers;
	public:
		MessageBus();
		~MessageBus();

		void registerReciever(MessageReciever *mr) {
			al_lock_mutex(mutexRecievers);
			messageRecievers.push_back(mr);
			al_unlock_mutex(mutexRecievers);
		}

		Message *createMessage();
		void pushMessage(Message*);

		void handleMessages();


		bool isEmpty() { return first == nullptr; }
		bool isFull() { return open == nullptr; }
	};


	void messageWindowLoadBitmap(Message *m, MemfileInfo info, ID window);

}