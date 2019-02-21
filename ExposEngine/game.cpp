#include "stdafx.h"
#include "game.h"

namespace expos {

	Game::Game() : gameConfig("EXPOSCONFIG"){
		al_init();
		al_init_image_addon();
		al_install_keyboard();
		al_install_mouse();
		al_install_joystick();
		al_init_primitives_addon();
		
		ID::create_mutex();

		queue = al_create_event_queue();

		logicTimer = al_create_timer(ALLEGRO_BPS_TO_SECS(60));
		al_register_event_source(queue, al_get_timer_event_source(logicTimer));

		ContentFile *graphicOptions = gameConfig.getCFSafe("window0");
		mainWindow = new Window(ID(ID_WINDOW), graphicOptions, queue);
	}

	Game::~Game() {
		delete mainWindow;

		al_destroy_event_queue(queue);
		al_destroy_timer(logicTimer);

		ID::destroy_mutex();
	}

	void *updateLoop(ALLEGRO_THREAD *thread, void *arg) {

		while (!al_get_thread_should_stop(thread)) {

		}

		return nullptr;
	}


	void Game::run() {
		ALLEGRO_THREAD *updateThread = al_create_thread(updateLoop, nullptr);

		al_start_timer(logicTimer);
		al_start_thread(updateThread);

		mainWindow->open();

		while (this->running) {
			this->checkAllegro();
		}

		mainWindow->close();

		al_join_thread(updateThread, nullptr);
		al_stop_timer(logicTimer);
		
		al_destroy_thread(updateThread);
	}

	void Game::checkAllegro() {
		ALLEGRO_EVENT ev;
		while (al_peek_next_event(queue, &ev)) {
			switch (ev.type) {
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				this->running = false;
				break;
			case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
				//al_clear_keyboard_state(display);
				break;/*
			case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
				this->drawingHalted = true;
				al_acknowledge_drawing_halt(display);
				al_stop_timer(logicTimer);
				break;
			case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
				this->drawingHalted = false;
				al_acknowledge_drawing_resume(display);
				al_start_timer(logicTimer);
				break;*/
			case ALLEGRO_EVENT_TIMER:
				if (ev.timer.source == logicTimer && al_get_timer_count(logicTimer) >= 2) {
					DrawList *drawList = mainWindow->getDrawList();
					drawlist::List *list = drawList->getUpdateList();

					list->startProduction();

					list->get()->primitive.id = ID(ID_PLINE)._ref();
					list->get()->primitive.a = { 50, 50 };
					list->get()->primitive.b = { 500, 530 };
					list->get()->primitive.col = al_map_rgb(255, 0, 255);
					list->get()->primitive.thickness = 3.2;

					list->productionFinished();

					drawList->ready(list);
					al_set_timer_count(this->logicTimer, 0);
					break;
				}
				break;
			}
		}
	}

}