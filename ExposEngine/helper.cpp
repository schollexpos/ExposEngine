#include "stdafx.h"
#include "helper.h"

namespace expos {
	std::map<std::string, ID> idMap;

	std::map<std::string, IDTYPE> generateIDTypeMap() {
		std::map<std::string, IDTYPE> map;
		map["str_"] = ID_STRING;
		map["bitm"] = ID_BITMAP;
		map["font"] = ID_FONT;
		map["anim"] = ID_ANIMATION;
		map["soun"] = ID_SOUND;
		map["musi"] = ID_MUSIC;
		map["movi"] = ID_MOVIE;
		map["scri"] = ID_SCRIPT;
		map["gpan"] = ID_GUIPANEL;
		map["acto"] = ID_ACTOR;
		map["item"] = ID_ITEM;
		map["room"] = ID_ROOM;
		return map;
	}

	IDTYPE getIDType(const std::string& str) {
		if (str.length() <= 4) return ID_END;

		static std::map<std::string, IDTYPE> htMap = generateIDTypeMap();

		auto it = htMap.find(str.substr(0, 4));
		if (it == htMap.end()) return ID_END;
		return it->second;
	}

	void exposError(const char *message) {
		unsigned int *peter = (unsigned int*)0xDEADBEEF;
		*peter = 0xDEADC0DE;
		//Crasht hier
	}

	/*
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
	}*/

	std::map<IDTYPE, IDKey> ID::used;

	void registerID(const std::string& id, ID h) {
		idMap[id] = h;
	}

	ID getID(const std::string& id) {
		auto it = idMap.find(id);

		if (it == idMap.end()) {
			ID tid(getIDType(id));
			registerID(id, tid);
			return tid;
		}

		return idMap.at(id);
	}
}