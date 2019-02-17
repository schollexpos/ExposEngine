#pragma once
#include "helper.h"

namespace expos {
	namespace gui {

		class Element {
		protected:
			ID id;
		public:
			Element(ID id) : id(id) {}

			ID getID() { return id; }
		};

		class Coordinate {
		private:
			int value;
		public:
			void parse(const std::string& str, Size<int> parent) {
				size_t pos;
				value = std::stoi(str, &pos);


			}
		};

		class Button : public Element {
		public:
			Button(ID id) : Element(id) {}
		};


	}
}
