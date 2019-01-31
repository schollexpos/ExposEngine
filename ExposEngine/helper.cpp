#include "stdafx.h"
#include "helper.h"

namespace expos {
	std::map<std::string, Handle> idMap;

	std::map<std::string, HANDLETYPE> generateHTMap() {
		std::map<std::string, HANDLETYPE> map;
		map["str_"] = H_STRING;
		map["bitm"] = H_BITMAP;
		map["font"] = H_FONT;
		map["anim"] = H_ANIMATION;
		map["soun"] = H_SOUND;
		map["musi"] = H_MUSIC;
		map["movi"] = H_MOVIE;
		map["scri"] = H_SCRIPT;
		map["gpan"] = H_GUIPANEL;
		map["acto"] = H_ACTOR;
		map["item"] = H_ITEM;
		map["room"] = H_ROOM;
		return map;
	}

	HANDLETYPE getHandleType(const std::string& str) {
		if (str.length() <= 4) return HANDLEEND;

		static std::map<std::string, HANDLETYPE> htMap = generateHTMap();

		auto it = htMap.find(str.substr(0, 4));
		if (it == htMap.end()) return HANDLEEND;
		return it->second;
	}

	void exposError(const char *message) {
		int *peter = (int*)0xDEADBEEF;
		*peter = 0xDEADC0DE;
		//Crasht hier
	}

	bool createCRCLookup(uint_fast16_t table[]) {
		uint_fast16_t remainder;
		for (size_t i = 0; i < 256; i++) {
			remainder = i;
			for (size_t j = 0; j < 8; j++) {
				if (remainder & 1) {
					remainder >>= 1;
					remainder ^= 0xedb88320;
				}
				else {
					remainder >>= 1;
				}
			}
			table[i] = remainder;
		}
		return true;
	}

	void registerID(const std::string& id, Handle h) {
		idMap[id] = h;
	}

	Handle getHandle(const std::string& id) {
		return idMap.at(id);
	}

}