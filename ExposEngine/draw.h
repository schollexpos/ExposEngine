#pragma once
#include "helper.h"

namespace expos {
	extern constexpr uint32_t DRAWLISTSIZE = 32;

	namespace drawlist {
		
		
		struct bmp {
			ID_ref id;
			Point<Pixel> position;
			Size<float> scaling, rotation;
		} ;

		struct primitive {
			ID_ref id;
			ALLEGRO_COLOR col;
			Point<Pixel> a, b, c;
			float thickness;

			void filledCircle(Point<Pixel> center, int radius, ALLEGRO_COLOR color) {
				a = center;
				b.x = radius;
				col = color;
			}

			void filledRect(Point<Pixel> a, Point<Pixel> b, ALLEGRO_COLOR color) {
				a = a;
				b = b;
				col = color;
			}
		} ;

		struct text {
			ID_ref textID, fontID;
			ALLEGRO_COLOR col;
			Point<Pixel> position;
		};

		union Entry {
			drawlist::bmp bmp;
			drawlist::primitive primitive;
			drawlist::text text;
		};

		class List {
		private:
			Entry *entrys;
			size_t curEntry = 0;
			size_t mysize = DRAWLISTSIZE; //not always the actual size, but just the filled
		public:
			List() {
				entrys = new Entry[mysize];
				mysize = 0;
			}

			~List() {
				delete[] entrys;
			}

			void reset() {
				curEntry = 0;
			}

			/* \brief "Consumes" the current entry (increments the entry-pointer and returns false if the end is reached) */
			bool consume() {
				curEntry++;
				return curEntry < mysize;
			}
			
			/* \brief "Produces" a new entry (increments the entry-pointer) */
			void produce() {
				curEntry++;
			}

			void startProduction() {
				this->resize(0);
				curEntry = 0;
			}

			void productionFinished() {
				this->resize(curEntry+1);
			}


			Entry *get() {
				return entrys + curEntry;
			}

			size_t size() { return mysize; }
			size_t maxSize() { return DRAWLISTSIZE; }
			/* \brief Sets the list to be filled with newsize elements
			*
			* Should be called by update after filling the drawing buffer
			*/
			void resize(size_t newsize) { mysize = newsize; }

		};

	}



	/* \brief A DrawList is used for fast communication between logic and drawing thread
	*
	*	It contains an array of DrawListEntry, in which the elements (bitmaps, primitives, fonts)
	*	that should be drawn next frame are added by the logic thread. The Drawing Thread then uses
	*	this to actually draw these to the screen.
	*/
	class DrawList {
	private:
		drawlist::List list[2];//double-buffered
		drawlist::List *update = list;
		drawlist::List *screen = list+1;
		ALLEGRO_MUTEX *mutex;
		ALLEGRO_COND *finished;
		int finCount = 0;
	public:
		DrawList() : mutex(al_create_mutex()), finished(al_create_cond()) {

		}

		/* \brief Blocks until all parties are finished with their list and swaps them
		*
		*	Must be called after the drawing/update thread have drawn/filled their drawing
		*	list and blocks the call until the other one is also finished, then swaps their
		*	lists (which then can be accessed by getUpdateList()/getScreenList()
		*/
		void ready(drawlist::List *list) {
			al_lock_mutex(mutex);
			finCount++;
			while (finCount == 1) {
				al_wait_cond(finished, mutex);
			}

			if (finCount == 2) {
				finCount = 0;
				drawlist::List *s = screen;
				screen = update;
				update = s;
				al_signal_cond(finished);
			}

			al_unlock_mutex(mutex);
		}


		drawlist::List *getUpdateList() {
			return update;
		}

		drawlist::List *getScreenList() {
			return screen;
		}
	};



}