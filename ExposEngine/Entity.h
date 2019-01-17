#pragma once
#include "helper.h"

namespace expos {
	class Entity;
	class Character;
	class Object;
	
	inline const HandleTableInstance<Object>& objectHT() {
		static HandleTableInstance<Object> table(H_OBJECT, 128);
		
	}

	class EntityRenderer {
		/*
			Contains Information
		*/
	private:
		Entity * owner;
	public:
		virtual void draw() {};
	};

	class EntityLogic {
	private:
		Entity * owner;
	public:
		virtual void update() {};
	};

	class Entity {
	private:
		EntityRenderer renderer;
		EntityLogic logic;
	public:

		virtual void check() = 0;
	};

	class EntityManager {
	private:
		HandleTableInstance<Entity> handleTable;
	public:

	};

}