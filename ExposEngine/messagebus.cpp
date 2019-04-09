#include "stdafx.h"
#include "messagebus.h"

namespace expos {

	MessageReciever::MessageReciever(MessageBus *mb, uint8_t recieving) : mb(mb), recieving(recieving) {
		if(mb) mb->registerReciever(this);
	}

	MessageBus::MessageBus() : mutexMQueue(al_create_mutex()), mutexRecievers(al_create_mutex()), mutexRequest(al_create_mutex()), messages(new Message[MAXMESSAGECOUNT]) {
		for (size_t i = 0; i < MAXMESSAGECOUNT-1;i++) {
			messages[i].next = &messages[i + 1];
			if (i == 0) this->open = &messages[i];
		}
	}

	MessageBus::~MessageBus() {
		al_destroy_mutex(mutexMQueue);
		al_destroy_mutex(mutexRecievers);
		al_destroy_mutex(mutexRequest);
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
		//Mögliche Racecond mit recievers, es wird angenommen, dass während des gameloop keine reciever hinzugefügt werden
		Message *m = first, *oldOpen;
		al_unlock_mutex(mutexMQueue);
		while (m) {
			std::cout << "MESSAGE" << std::endl;
			for (size_t i = 0; i < messageRecievers.size();i++) {
				if (messageRecievers[i]->getRecieving() & m->recievers) {
					
					if (messageRecievers[i]->recieve(*m) || i == messageRecievers.size() - 1) { 
						al_lock_mutex(mutexMQueue);
						oldOpen = open;
						open = m;
						m->next = oldOpen;
						al_unlock_mutex(mutexMQueue);
					} 
					
				}
			}

			m = m->next;
		}

		first = nullptr;
	}

	bool MessageBus::sendRequest(const Message &m, Answer *a) {
		al_lock_mutex(mutexRequest);
		a->success = false;
		for (MessageReciever *r : messageRecievers) {
			if (r->getRecieving() & m.recievers) {
				if (r->answerRequest(m, a)) {
					al_unlock_mutex(mutexRequest);
					return true;
				}
			}
		}

		al_unlock_mutex(mutexRequest);
		return true;
	}



	/* Message-Builder */

	void messageWindowLoadBitmap(Message *m, MemfileInfo info, ID window) {
		m->type = MESSAGE_WINDOW_LOADBITMAP;
		m->recievers = RECIEVER_DISPLAY;
		m->data.window.m = info;
		m->data.window.window = window._ref();
	}

	void messageFileManagerLoadPF(Message *m, ID filename) {
		m->type = MESSAGE_FILEMANAGER_LOADPF;
		m->recievers = RECIEVER_FILEMAN;
		m->data.filemanager.filename = filename._ref();
	}

}