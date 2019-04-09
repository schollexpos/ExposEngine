#include "stdafx.h"
#include <allegro5/allegro_primitives.h>
#include "window.h"


namespace expos {

	void *windowThread(ALLEGRO_THREAD *thread, void* arg) {
		/* Takes:
		DrawListEntry[DRAWLISTSIZE]
		CONTENTFILE* (containing init data)
		int *frame;
		ALLEGRO_MUTEX *mutex;
		*/
		void **arguments = (void**)arg;
		ALLEGRO_DISPLAY *display;

		DrawList *drawList = ((DrawList*)arguments[0]);
		ALLEGRO_EVENT_QUEUE *eventQueue = ((ALLEGRO_EVENT_QUEUE*)arguments[1]);
		ContentFile *graphicOptions = ((ContentFile*)arguments[2]);

		//delete arguments;
		
		/*
		int screenMode = (graphicOptions->getInt("fullscreen", 0) != 0 ? ALLEGRO_FULLSCREEN_WINDOW : ALLEGRO_WINDOWED);
		al_set_new_display_flags(screenMode | ALLEGRO_OPENGL | ALLEGRO_PROGRAMMABLE_PIPELINE);

		int optionMode = (graphicOptions->getInt("_force_options", 0) != 0 ? ALLEGRO_REQUIRE : ALLEGRO_SUGGEST);

		al_set_new_display_option(ALLEGRO_VSYNC, graphicOptions->getInt("vsync", 1), optionMode);
		int bmpFlags = 0;
		if (graphicOptions->getInt("linear_filtering", 1)) bmpFlags |= ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR;
		if (graphicOptions->getInt("mipmap", 0)) bmpFlags |= ALLEGRO_MIPMAP;

		al_set_new_bitmap_flags(bmpFlags);*/
		
		display = al_create_display(1280, 720);
		al_register_event_source(eventQueue, al_get_display_event_source(display));

		std::map<ID, ALLEGRO_BITMAP*> bitmaps;

		int myFrame = 0;
		while (!al_get_thread_should_stop(thread)) {

			drawlist::List *drawbuffer = drawList->getScreenList();

			do {
				drawlist::Entry *entry = drawbuffer->get();

				if (entry->primitive.id.part.t == ID_INVALID) break;

				switch (entry->primitive.id.part.t) {
				case ID_BITMAP:

					break;
				case ID_FONT:

					break;
				case ID_PLINE:
					al_draw_line(entry->primitive.a.x, entry->primitive.a.y, entry->primitive.b.x, entry->primitive.b.y, entry->primitive.col, entry->primitive.thickness);
					break;
				default:
					std::cout << "DEFAULT!" << std::endl;
					break;
				}
			} while ((drawbuffer->consume()));

			drawList->ready(drawbuffer);
			al_flip_display();
		}

		return nullptr;
	}


	Window::Window(ID id, MessageBus *mb, ContentFile *windowConfig, ALLEGRO_EVENT_QUEUE *queue) : MessageReciever(mb, RECIEVER_DISPLAY), id(id) {
		void **arguments = new void*[3] { (void*)&drawList, (void*)queue, (void*)windowConfig};
		thread = al_create_thread(windowThread, arguments);
	}

	Window::~Window() {
		al_destroy_thread(thread);
	}


	void Window::open() {
		al_start_thread(thread);
	}

	void Window::close() {
		al_join_thread(thread, nullptr);
	}

	bool Window::recieve(const Message& message) {
		return false;
	}

	bool Window::answerRequest(const Message& request, Answer *answer) {
		switch(request.type) {
		case REQUEST_TEXT_SIZE:
			answer->success = false;
			answer->data.position.size = { 0, 0 };
			return true;
		default:
			return false;
		}
		
	}
}