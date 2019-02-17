#include "stdafx.h"
#include "messagebus.h"

namespace expos {
	MessageBus::MessageBus() : mutexMQueue(al_create_mutex()), mutexRecievers(al_create_mutex()), messages(new Message[MAXMESSAGECOUNT]) {
		for (size_t i = 0; i < MAXMESSAGECOUNT-1;i++) {
			messages[i].next = &messages[i + 1];
			if (i == 0) this->open = &messages[i];
		}
	}

	MessageBus::~MessageBus() {
		delete[] messages;
	}

	Message *MessageBus::createMessage() {
		al_lock_mutex(mutexMQueue);

		Message *m = open;
		if (open == nullptr) return nullptr;
		open = open->next;

		al_unlock_mutex(mutexMQueue);
		return m;
	}

	void MessageBus::pushMessage(Message *m) {
		al_lock_mutex(mutexMQueue);
		Message *oldFirst = first;
		first = m;
		first->next = m;
		al_unlock_mutex(mutexMQueue);
	}

	void MessageBus::handleMessages() {
		if (this->isEmpty()) return;
		
		al_lock_mutex(mutexMQueue);
		//M�gliche Racecond mit recievers, es wird angenommen, dass w�hrend des gameloop keine reciever hinzugef�gt werden
		Message *m = first, *oldOpen;
		al_unlock_mutex(mutexMQueue);
		while (m) {
			for (size_t i = 0; i < messageRecievers.size();i++) {
				if (messageRecievers[i]->getRecieving() & m->recievers) {
					
					if (messageRecievers[i]->recieve(m) || i == messageRecievers.size() - 1) { 
						al_lock_mutex(mutexMQueue);
						oldOpen = open;
						open = m;
						m->next = oldOpen;
						al_unlock_mutex(mutexMQueue);
					} 
					
				}
			}

		}
	}



	/* Message-Builder */

	void messageWindowLoadBitmap(Message *m, MemfileInfo info, ID window) {
		m->type = MESSAGE_WINDOW_LOADBITMAP;
		m->recievers = RECIEVER_DISPLAY;
		m->data.window.m = info;
		m->data.window.window = window._ref();
	}

}