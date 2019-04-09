#pragma once
#include <allegro5/allegro.h>
#include "helper.h"
#include "draw.h"
#include "contentfile.h"
#include "messagebus.h"

namespace expos {
	


	void *windowThread(ALLEGRO_THREAD*, void*);

	class Window : public MessageReciever {
	private:
		ID id;
		DrawList drawList;
		ALLEGRO_THREAD *thread;
	public:
		Window(ID id, MessageBus*, ContentFile*, ALLEGRO_EVENT_QUEUE*);
		~Window();

		void open();
		void close();

		bool recieve(const Message&);
		bool answerRequest(const Message&, Answer*);

		DrawList *getDrawList() { return &drawList; }

		ID getID() { return id; }

	};

}
