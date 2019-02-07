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

		queue = al_create_event_queue();

		logicTimer = al_create_timer(ALLEGRO_BPS_TO_SECS(60));
		al_register_event_source(queue, al_get_timer_event_source(logicTimer));

		ContentFile *graphicOptions = gameConfig.getCFSafe("window0");
		mainWindow = new Window(graphicOptions, queue);
	}

	Game::~Game() {
		delete mainWindow;

		al_destroy_event_queue(queue);
		al_destroy_timer(logicTimer);
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
					auto *list = mainWindow->lockDraw();
					list[0].PRIMITIVE.h = handle(H_PLINE);
					list[0].PRIMITIVE.a = { 50, 50 };
					list[0].PRIMITIVE.b = { 500, 530 };
					list[0].PRIMITIVE.col = al_map_rgb(255, 0, 255);
					list[0].PRIMITIVE.thickness = 3.2;

					list[1].PRIMITIVE.h = handle(H_INVALID);

					mainWindow->unlockDraw();
					al_set_timer_count(this->logicTimer, 0);
					break;
				}
				break;
			}
		}
	}

}