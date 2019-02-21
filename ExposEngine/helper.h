#pragma once
#include <cstdint>
#include <cassert>
#include <sstream>
#include "stdafx.h"
#include <allegro5/allegro.h>


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

	std::vector<std::string> explode(const std::string& str, char delimiter);
	std::string implode(const std::vector <std::string>& vec, char delimiter, int start = 0);

	enum IDTYPE : std::uint16_t {
		ID_INVALID = 0x0000,
		ID_STRING = 0x0001,
		ID_WINDOW = 0x0002,

		ID_PPOINT = 0x0500,
		ID_PLINE = 0x0501,
		ID_PTRIANGLE = 0x0502,
		ID_PFILLTRIANGLE = 0x0503,
		ID_PRECT = 0x0504,
		ID_PFILLRECT = 0x0505,
		ID_PCIRCLE = 0x0506,
		ID_PFILLCIRCLE = 0x0507,
		

		ID_BITMAP = 0x1000,
		ID_FONT = 0x1001,
		ID_ANIMATION = 0x1002,
		ID_SOUND = 0x1003,
		ID_MUSIC = 0x1004,
		ID_MOVIE = 0x1005,

		ID_SCRIPT = 0x2000,

		ID_GUIPANEL = 0x3000,
		ID_GUILABEL = 0x3001,
		ID_GUIBUTTON = 0x3002,

		ID_ACTOR = 0x4000,
		ID_ITEM = 0x4000,
		ID_ROOM = 0x4000,

		ID_END = 0xFFFF
	};

	std::map<std::string, IDTYPE> generateIDTypeMap();
	IDTYPE getIDType(const std::string& str);

	typedef uint16_t IDKey;

	class ID;

	extern std::map<std::string, ID> idMap;

	void registerID(const std::string& id, ID h);
	ID getID(const std::string& id);

	union ID_ref {
		std::uint32_t v;
		struct {
			IDKey v;
			IDTYPE t;
		} part;
	};

	/*
	class Semaphore {
	private:
		int n;
		ALLEGRO_MUTEX *mutex;
		ALLEGRO_COND *blocked;
	public:
		Semaphore(int n) : n(n), mutex(al_create_mutex()), blocked(al_create_cond()) {}

		~Semaphore() {
			al_destroy_mutex(mutex);
			al_destroy_cond(blocked);
		}

		void wait() {
			al_lock_mutex(mutex);
			while (n == 0) {
				al_wait_cond(blocked, mutex);
			}
			n--;
			al_unlock_mutex(mutex);
		}

		void signal() {
			al_lock_mutex(mutex);
			n++;
			al_signal_cond(blocked);
			al_unlock_mutex(mutex);
		}
	};*/

	class ID {
	private: 
		ID_ref val;
		static std::map<IDTYPE, IDKey> used;
		static ALLEGRO_MUTEX *mutex;
	public:
		ID() {}

		ID(IDTYPE t) {
			this->val.part.t = t;

			al_lock_mutex(mutex);
			auto it = used.find(t);
			if (it == used.end()) {
				used.insert(std::pair<IDTYPE, IDKey>(t, (IDKey)0));
				this->val.part.v = 0;
			}
			else {
				this->val.part.v = it->second++;
			}
			al_unlock_mutex(mutex);
		}

		ID(const ID& id) {
			this->val = id();
		}

		ID_ref operator()() const {
			return this->val;
		}

		ID_ref _ref() {
			return this->val;
		}

		static void create_mutex() { mutex = al_create_mutex(); }
		static void destroy_mutex() { al_destroy_mutex(mutex); }

		ID(const std::string& str) : ID(getID(str)) {}
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

	/*
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

	};*/

}