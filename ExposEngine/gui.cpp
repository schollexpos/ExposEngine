#include "stdafx.h"
#include "gui.h"
#include "contentfile.h"


namespace expos {
	namespace gui {

		MessageBus* Element::mb = nullptr;

		/* Takes an html color in the format #RRGGBB or #RRGGBBAA (the hash symbol is non-mandatory) and converts it
		*
		* al_color_html only accepts RGB values
		*/
		ALLEGRO_COLOR html2color(const std::string& col) {
			size_t pos = 0;
			if (col[pos] == '#') pos++;
			unsigned char comp[4] = {0,0,0,0};
			int c = 0;
			while (pos < col.size() && c < 4) {
				comp[c] = std::stoi(col.substr(pos, 2));
				pos += 2;
				c++;
			}
			return al_map_rgba(comp[0], comp[1], comp[2], comp[3]);
		}

		Point<Pixel> parsePoint(Size<Pixel> parent, const std::string& x, const std::string& y) {
			return {Coordinate::parse(x, parent, true), Coordinate::parse(y, parent, false)};
		}

		Label::Label(ContentFile* cf, const Element& parent) : 
			Element(ID(ID_GUILABEL), parsePoint(parent.getSize(), cf->getValue("x"), cf->getValue("y"))),
			font(ID(cf->getValRecursive("font"))),
			text(cf->getValue("text", "null")),
			color(html2color(cf->getValRecursive("color"))) {

		}

		void Label::draw(drawlist::List *list) {
			list->get()->text = { this->font._ref(), this->text._ref(), this->color, this->getPos() };
			list->produce();
		}

		Button::Button(ContentFile *cf, const Element& parent) :
			Element(ID(ID_GUILABEL), parsePoint(parent.getSize(), cf->getValue("x"), cf->getValue("y"))),
			color(html2color(cf->getValRecursive("color"))),
			bgcolor(html2color(cf->getValRecursive("bgcolor"))),
			hlcolor(html2color(cf->getValRecursive("highlighted"))),
			text(parsePoint(this->getSize(), "2%", "2%"), ID(cf->getValRecursive("font")), cf->getValue("text", "null"), this->color) {


			Pixel margin = (int) ((double)text.getSize().h * 0.10);
			size.w = (double) text.getSize().w + margin;
			size.h = (double) text.getSize().h + margin;
		}

		void Button::draw(drawlist::List *list) {
			list->get()->primitive.filledCircle(
				{this->pos.x + this->getSize().h / 2, this->pos.y + this->getSize().h / 2 }, this->getSize().h / 2, this->bgcolor);
			list->get()->primitive.filledCircle(
				{this->pos.x + this->getSize().w - this->getSize().h / 2, this->pos.y + this->getSize().h / 2 }, this->getSize().h / 2, this->bgcolor);
			list->get()->primitive.filledRect({ this->pos.x + this->getSize().h / 2 ,0 }, { this->pos.x + this->getSize().w - this->getSize().h / 2, this->getSize().h }, this->bgcolor);
			list->produce();
		}


	}
}