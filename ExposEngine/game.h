#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "contentfile.h"
#include "window.h"

namespace expos {

	class Game {
	private:
		ContentFile gameConfig;
		ALLEGRO_EVENT_QUEUE * queue;
		ALLEGRO_TIMER *logicTimer;
		Window *mainWindow;
		bool drawingHalted = false;
		bool running = true;
	public:
		Game();
		~Game();

		void run();
		

		void checkAllegro();





	};

}