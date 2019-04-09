#pragma once
#include "stdafx.h"
#include <allegro5/allegro.h>
#include "helper.h"

namespace expos {

	enum MESSAGETYPE {
		MESSAGE_DISPLAYEVENT,
		MESSAGE_WINDOW_LOADBITMAP,
		MESSAGE_FILEMANAGER_LOADPF,
		REQUEST_TEXT_SIZE
	};

constexpr auto RECIEVER_GAME = 0x01;
constexpr auto RECIEVER_FILEMAN = 0x02;
constexpr auto RECIEVER_DISPLAY = 0x04;
constexpr auto  RECIEVER_AUDIO = 0x08;
constexpr auto  RECIEVER_LOGIC = 0x10;
constexpr auto  RECIEVER_SCRIPT = 0x20;
	
	union MessageData {
		struct {
			uint32_t val1, val2, val3, val4;
		} general;
		struct {
			ID_ref window, bitmap;
			MemfileInfo m;
		} window;
		struct {
			ID_ref font, text;
		} font;
		struct {
			ID_ref filename;
		} filemanager;
		struct {
			Point<Pixel> pos;
			Size<Pixel> size;
		} position;
	};


	struct Message {
		MessageData data;
		Message *next;
		MESSAGETYPE type;
		uint8_t recievers;
	};

	struct Answer {
		MessageData data;
		bool success;
	};

	class MessageBus;

	class MessageReciever {
	protected:
		MessageBus *mb;
		uint8_t recieving;
	public:
		MessageReciever(MessageBus *mb, uint8_t recieving);

		//returns true if the event was consumed and shouldn't be propagated further
		virtual bool recieve(const Message&) = 0;
		
		//returns true if the message was answered (this can be either success or failure)
		virtual bool answerRequest(const Message&, Answer*) = 0;

		uint8_t getRecieving() { return recieving; }

	};

	class MessageBus {
	private:
		ALLEGRO_MUTEX * mutexMQueue, *mutexRecievers, *mutexRequest;
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
		bool sendRequest(const Message&, Answer *);


		bool isEmpty() { return first == nullptr; }
		bool isFull() { return open == nullptr; }
	};


	void messageWindowLoadBitmap(Message *m, MemfileInfo info, ID window);
	void messageFileManagerLoadPF(Message *m, ID filename);

}