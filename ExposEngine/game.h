#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include "contentfile.h"

namespace expos {

	class Game {
	private:
		ContentFile gameConfig;
		ALLEGRO_EVENT_QUEUE * queue;
		ALLEGRO_DISPLAY *display;
		ALLEGRO_TIMER *logicTimer;

		bool doRedraw = true;
		bool drawingHalted = false;
		bool running = true;
	public:
		Game();
		~Game();

		void run();
		
		void draw();
		void checkAllegro();





	};

}