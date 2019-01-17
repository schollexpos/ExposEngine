#pragma once
#include <cstdint>
#include <cassert>
#include "stdafx.h" 

namespace expos {

	class Bitmap;
	class Font;
	class Animation;
	class Sound;
	class Music;
	class Movie;
	class Script;
	class GuiPanel;
	class Character;
	class Object;
	class Room;

	enum HANDLETYPE : std::uint16_t {
		H_INVALID = 0x0000,
		H_STRING = 0x0001,

		H_BITMAP = 0x0002,
		H_FONT = 0x0003,
		H_ANIMATION = 0x0004,
		H_SOUND = 0x0005,
		H_MUSIC = 0x0006,
		H_MOVIE = 0x0007,

		H_SCRIPT = 0x0008,

		H_GUIPANEL = 0x0009,

		H_ACTOR = 0x0010,
		H_ITEM = 0x0011,
		H_ROOM = 0x0012,

		HANDLEEND = 0x0013
	};

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

	union Handle {
		std::uint32_t v;
		struct {
			std::uint16_t v;
			HANDLETYPE t;
		} part;
	};

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
				} else {
					remainder >>= 1;
				}
			}
			table[i] = remainder;
		}
		return true;
	}

	uint_fast16_t crc(const std::string& str) {
		static uint_fast16_t lookupTable[256];
		static bool success = createCRCLookup(lookupTable);

		uint_fast16_t crc16 = 0xFFFFFFFF;
		
		for (size_t i = 0; i < str.length; i++) {
			int index = (crc16 ^ str[i]) & 0xFF;
			crc16 = (crc16 >> 8) ^ lookupTable[index];
		}

		return crc16;
	}

	Handle getHandle(const std::string& id) {
		auto hash = crc(id);
		static std::map<uint_fast16_t, Handle> hMap;
		while (hMap.find(hash) != hMap.end()) {
			hash++;
		}

		Handle h;
		h.part.v = hash;
		h.part.t = getHandleType(id);

		return h;
	}
}