#pragma once
#include <allegro5/allegro.h>
#include "helper.h"
#include "contentfile.h"
#include "messagebus.h"

namespace expos {
	extern constexpr uint32_t DRAWLISTSIZE = 32;

	union DrawListEntry {
		struct {
			ID_ref h;
			Point<Pixel> position;
			Size<float> scaling, rotation;
		} BMP;
		struct {
			ID_ref h;
			ALLEGRO_COLOR col;
			Point<Pixel> a, b, c;
			float thickness;
		} PRIMITIVE;
		struct {
			ID_ref h;
			ALLEGRO_COLOR col;
			Point<Pixel> position;
		} TEXT;
	};


	void *windowThread(ALLEGRO_THREAD*, void*);

	class Window : public MessageReciever {
	private:
		ID id;
		DrawListEntry drawList[DRAWLISTSIZE];
		ALLEGRO_THREAD *thread;
		ALLEGRO_MUTEX *mutex;
		int frame = 0;
	public:
		Window(ID id, ContentFile*, ALLEGRO_EVENT_QUEUE*);
		~Window();

		void open();
		void close();

		bool recieve(Message*);

		DrawListEntry *lockDraw();
		void unlockDraw();

		ID getID() { return id; }

	};

}
