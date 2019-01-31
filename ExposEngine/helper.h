#pragma once
#include <cstdint>
#include <cassert>
#include "stdafx.h" 
class ContentFile;
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

	template<typename T>
	struct Point {
		T x, y;
	};

	template<typename T>
	struct Rect {
		Point<T> origin, size;
	};

	enum DIRECTION {
		DIR_LEFT,
		DIR_RIGHT,
		DIR_UP,
		DIR_DOWN
	};

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

	std::map<std::string, HANDLETYPE> generateHTMap();
	HANDLETYPE getHandleType(const std::string& str);

	union Handle {
		std::uint32_t v;
		struct {
			std::uint16_t v;
			HANDLETYPE t;
		} part;
	};

	void exposError(const char *message);

	bool createCRCLookup(uint_fast16_t table[]);

	/*uint_fast16_t crc(const std::string& str) {
		static uint_fast16_t lookupTable[256];
		static bool success = createCRCLookup(lookupTable);

		uint_fast16_t crc16 = 0xFFFFFFFF;
		
		for (size_t i = 0; i < str.length; i++) {
			int index = (crc16 ^ str[i]) & 0xFF;
			crc16 = (crc16 >> 8) ^ lookupTable[index];
		}

		return crc16;
	}*/


	extern std::map<std::string, Handle> idMap;

	void registerID(const std::string& id, Handle h);
	Handle getHandle(const std::string& id);

	template<typename T, HANDLETYPE type, size_t n>
	class HandleKeeper {
	private:
		T *handles[n];
		size_t open = 0;
	public:
		Handle registerObject(const std::string& id, T *object) {
			Handle h = this->registerObject(object);
			registerID(id, h);
			return h;
		}

		Handle registerObject(T *object) {
			handles[open] = object;
			Handle h;
			h.part.t = type;
			h.part.v = open++;
			return h;
		}

		T *operator[](Handle h) {
			return handles[h.part.v];
		}

	};

}