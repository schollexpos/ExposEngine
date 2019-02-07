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
	struct Size {
		T w, h;
	};

	template<typename T>
	struct Rect {
		Point<T> origin;
		Size<T> size;
	};

	struct MemfileInfo {
		void *mem; 
		int32_t size;
	};

	typedef uint32_t Pixel;

	enum DIRECTION {
		DIR_LEFT,
		DIR_RIGHT,
		DIR_UP,
		DIR_DOWN
	};

	enum HANDLETYPE : std::uint16_t {
		H_INVALID = 0x0000,
		H_STRING = 0x0001,

		H_PPOINT = 0x0500,
		H_PLINE = 0x0501,
		H_PTRIANGLE = 0x0502,
		H_PFILLTRIANGLE = 0x0503,
		H_PRECT = 0x0504,
		H_PFILLRECT = 0x0505,
		H_PCIRCLE = 0x0506,
		H_PFILLCIRCLE = 0x0507,
		

		H_BITMAP = 0x1000,
		H_FONT = 0x1001,
		H_ANIMATION = 0x1002,
		H_SOUND = 0x1003,
		H_MUSIC = 0x1004,
		H_MOVIE = 0x1005,

		H_SCRIPT = 0x2000,

		H_GUIPANEL = 0x3000,

		H_ACTOR = 0x4000,
		H_ITEM = 0x4000,
		H_ROOM = 0x4000,

		HANDLEEND = 0xFFFF
	};

	std::map<std::string, HANDLETYPE> generateHTMap();
	HANDLETYPE getHandleType(const std::string& str);

	typedef uint16_t HandleIndex;

	union Handle {
		std::uint32_t v;
		struct {
			HandleIndex v;
			HANDLETYPE t;
		} part;
	};

	inline Handle handle(HANDLETYPE t) {
		Handle h;
		h.part.t = t;
		return h;
	}

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