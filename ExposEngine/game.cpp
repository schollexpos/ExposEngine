#include "stdafx.h"

#include "game.h"

namespace expos {

	Game::Game() : gameConfig("EXPOSCONFIG"){
		al_init();
		al_init_image_addon();
		al_install_keyboard();
		al_install_mouse();
		al_install_joystick();

		queue = al_create_event_queue();

		logicTimer = al_create_timer(ALLEGRO_BPS_TO_SECS(60));

		ContentFile *graphicOptions = gameConfig.getCFSafe("graphic");

		std::cout << graphicOptions << std::endl;
		
		int screenMode = (graphicOptions->getInt("fullscreen", 0) != 0 ? ALLEGRO_FULLSCREEN_WINDOW : ALLEGRO_WINDOWED);
		al_set_new_display_flags(screenMode | ALLEGRO_OPENGL | ALLEGRO_PROGRAMMABLE_PIPELINE);

		int optionMode = (graphicOptions->getInt("_force_options", 0) != 0 ? ALLEGRO_REQUIRE : ALLEGRO_SUGGEST);

		al_set_new_display_option(ALLEGRO_VSYNC, graphicOptions->getInt("vsync",  1), optionMode);
		int bmpFlags = 0;
		if (graphicOptions->getInt("linear_filtering", 1)) bmpFlags |= ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR;
		if (graphicOptions->getInt("mipmap", 0)) bmpFlags |= ALLEGRO_MIPMAP;

		al_set_new_bitmap_flags(bmpFlags);

		display = al_create_display(1280, 720);
		al_register_event_source(queue, al_get_display_event_source(display));
	}

	Game::~Game() {
		al_destroy_event_queue(queue);
		al_destroy_timer(logicTimer);
	}

	void *updateLoop(ALLEGRO_THREAD *thread, void *arg) {

		return nullptr;
	}

	void Game::run() {
		ALLEGRO_THREAD *updateThread = al_create_thread(updateLoop, nullptr);

		al_start_timer(logicTimer);
		al_start_thread(updateThread);

		while (this->running) {
			this->checkAllegro();
			this->draw();

			al_flip_display();
		}

		al_join_thread(updateThread, nullptr);
		al_stop_timer(logicTimer);
		
		al_destroy_thread(updateThread);
	}

	void Game::draw() {
		if (!this->doRedraw) return;

		this->doRedraw = false;
	}

	void Game::checkAllegro() {
		ALLEGRO_EVENT ev;
		while (al_get_next_event(queue, &ev)) {
			switch (ev.type) {
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				this->running = false;
				break;
			case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
				//al_clear_keyboard_state(display);
				break;
			case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
				this->drawingHalted = true;
				al_acknowledge_drawing_halt(display);
				al_stop_timer(logicTimer);
				break;
			case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
				this->drawingHalted = false;
				al_acknowledge_drawing_resume(display);
				al_start_timer(logicTimer);
				break;
			case ALLEGRO_EVENT_TIMER:
				if (ev.timer.source == logicTimer && al_get_timer_count(logicTimer) >= 2) {
					al_set_timer_count(this->logicTimer, 0);
					break;
				}
				break;
			}
		}
	}

}