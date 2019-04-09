#pragma once
#include "allegro5/allegro.h"
#include "helper.h"
#include "draw.h"
#include "messagebus.h"

namespace expos {
	namespace gui {

		class Element {
		protected:
			static MessageBus *mb;
			ID id;
			Point<Pixel> pos;
			Size<Pixel> size;
		public:
			Element(ID id, Point<Pixel> pos) : id(id), pos(pos) {}

			virtual void draw(drawlist::List *list) = 0;

			ID getID() const { return id; }
			Point<Pixel> getPos() const { return pos;  };
			virtual Size<Pixel> getSize() const { return size; };

			static void setMB(MessageBus *mb) {
				Element::mb = mb;
			}
		};


		/*
			Gui Coordinates are always stored as pixel information, regardless their
			representation in the ContentFiles
		*/
		class Coordinate {
		public:
			/*	Parses a Coordinate from the string str. Example: "355px" or "56%w"
			*	Accepted units are either px (Pixels, if no unit is given this is assumed) or
			*	percent of the parent element's size. A 'w' or 'h' indicates wether to use
			*	the width or height of the parent.
			*/
			static Pixel parse(const std::string& str, Size<Pixel> parent, bool width = false) {
				size_t pos;
				int value = std::stoi(str, &pos);

				if (str.length() > pos) {
					if (str[pos] == '%') {
						if (str.length() > pos + 1) {
							width = str[pos + 1] == 'w';
						}

						if (width) {
							value = (Pixel)((double)value / 100.0 * parent.w);
						} else {
							value = (Pixel)((double)value / 100.0 * parent.h);
						}
					}
				}

				return value;
			}
		};

		Point<Pixel> parsePoint(Size<Pixel> parent, const std::string& x, const std::string& y);

		/* Label Element 
		* Displays a small text label in other elements, and on its self
		* Parameters in ContentFile:
		* - x,y  (int)
		* - font (string)
		* - text (string)
		* - color (color)
		*/
		class Label : public Element {
		private:
			ID font, text;
			ALLEGRO_COLOR color;

			void setSize() {
				Message req;
				req.type = REQUEST_TEXT_SIZE;
				req.recievers = RECIEVER_DISPLAY;
				req.data.font = { font._ref(), text._ref() };
				Answer a;

				mb->sendRequest(req, &a);
				if (a.success) {
					size = a.data.position.size;
				}
				else {
					size = { 0, 0 };
				}
			}
		public:
			Label(Point<Pixel> pos, ID font, ID text, ALLEGRO_COLOR color) : Element(ID(ID_GUILABEL), pos), font(font), text(text), color(color) {}
			Label(ContentFile*, const Element&);

			void draw(drawlist::List *list);
		};

		/* A standard Gui Button
		* Parameters in ContentFile:
		* - x,y (int)
		* - text (text)
		* - color, bgcolor, highlighted color (color)
		*/
		class Button : public Element {
		private:
			ALLEGRO_COLOR color, bgcolor, hlcolor;
			Label text;
		public:
			Button(ContentFile*, const Element&);

			void draw(drawlist::List *list);
		};


	}
}
