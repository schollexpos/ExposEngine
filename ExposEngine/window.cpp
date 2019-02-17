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

		DrawListEntry *drawList = ((DrawListEntry*)arguments[0]);
		ALLEGRO_EVENT_QUEUE *eventQueue = ((ALLEGRO_EVENT_QUEUE*)arguments[1]);
		ContentFile *graphicOptions = ((ContentFile*)arguments[2]);
		int *frameToDisplay = ((int*)arguments[3]);
		ALLEGRO_MUTEX *mutex = ((ALLEGRO_MUTEX*)arguments[4]);

		delete arguments;
		
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

			if (myFrame != *frameToDisplay) {
				al_lock_mutex(mutex);
				myFrame = *frameToDisplay;
				
				for (size_t i = 0; i < DRAWLISTSIZE; i++) {
					if (drawList[i].PRIMITIVE.h.part.t == ID_INVALID) break;

					switch (drawList[i].PRIMITIVE.h.part.t) {
					case ID_BITMAP:

						break;
					case ID_FONT:

						break;
					case ID_PLINE:
						al_draw_line(drawList[i].PRIMITIVE.a.x, drawList[i].PRIMITIVE.a.y, drawList[i].PRIMITIVE.b.x, drawList[i].PRIMITIVE.b.y, drawList[i].PRIMITIVE.col, drawList[i].PRIMITIVE.thickness);
						break;
					}
				}
				
				al_flip_display();
				al_unlock_mutex(mutex);
			}
		}

		return nullptr;
	}


	Window::Window(ID id, ContentFile *windowConfig, ALLEGRO_EVENT_QUEUE *queue) : id(id), mutex(al_create_mutex()) {
		void **arguments = new void*[5] { (void*)drawList, (void*)queue, (void*)windowThread, (void*) &frame, (void*) mutex};
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

	bool Window::recieve(Message* message) {
		return false;
	}


	DrawListEntry *Window::lockDraw() {
		al_lock_mutex(mutex);
		return drawList;
	}

	void Window::unlockDraw() {
		frame++;
		if (frame > 30) frame = 0;
		al_unlock_mutex(mutex);
	}
}