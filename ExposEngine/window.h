#pragma once
#include <allegro5/allegro.h>
#include "helper.h"
#include "contentfile.h"
#include "messagebus.h"

namespace expos {
	extern constexpr uint32_t DRAWLISTSIZE = 32;

	union DrawListEntry {
		struct {
			Handle h;
			Point<Pixel> position;
			Size<float> scaling, rotation;
		} BMP;
		struct {
			Handle h;
			ALLEGRO_COLOR col;
			Point<Pixel> a, b, c;
			float thickness;
		} PRIMITIVE;
		struct {
			Handle h;
			ALLEGRO_COLOR col;
			Point<Pixel> position;
		} TEXT;
	};


	void *windowThread(ALLEGRO_THREAD*, void*);

	class Window : public MessageReciever {
	private:
		Handle h;
		DrawListEntry drawList[DRAWLISTSIZE];
		ALLEGRO_THREAD *thread;
		ALLEGRO_MUTEX *mutex;
		int frame = 0;
	public:
		Window(Handle h, ContentFile*, ALLEGRO_EVENT_QUEUE*);
		~Window();

		void open();
		void close();

		bool recieve(Message*);

		DrawListEntry *lockDraw();
		void unlockDraw();

		Handle getHandle() { return h; }

	};

}
