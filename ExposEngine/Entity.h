#pragma once
#include "stdafx.h"
#include "helper.h"
#include "animation.h"

namespace expos {
	class Entity;
	class Actor;
	class Item;

/*
	LOCKED: bleibt geladen
	DISABLED: Kein Mouseover möglich
	INVISIBLE: Kein Draw
*/
#define EFLAG_LOCKED	0x01
#define EFLAG_DISABLED	0x02
#define EFLAG_INVISIBLE	0x04

	struct EntityData {
		AnimationInstance curAnim;
		Rect<int> position;
		int zIndex;
		uint8_t flags;
	};

	class Entity {
	protected:
		EntityData edata;
	public:

		virtual void check() = 0;
	};

	enum ITEMSTATE {
		ITST_IDLE,
		ITST_PICKEDUP
	};

	class Item : public Entity {
	private:


		ITEMSTATE state;
	public:

	};

	class Inventory {

	};

	struct ActorAnimationSet {
		ID standing[4];
		ID walking[4];
		ID talking[4];
	};
	
	enum ACTORSTATE {
		ACST_IDLE,
		ACST_WALKING,
		ACST_TALKING
	};

	class Actor : public Entity {
	private:
		Inventory inventory;
		std::vector<ActorAnimationSet> animationSets;
		ID dialogue;
		ACTORSTATE state;
	public:
	};

	class EntityManager {
	private:
	public:

	};

}