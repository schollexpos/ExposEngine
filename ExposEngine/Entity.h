#pragma once

class Entity;

class EntityRenderer {
	/* 
		Contains Information
	*/
private:
	Entity *owner;
public:
	virtual void draw() = 0;
};

class EntityLogic {
private:
	Entity *owner;
public:
	virtual void update() = 0;
};

class Entity {
private:
	EntityRenderer *renderer;
	EntityLogic *logic;
public:

	virtual void check() = 0;
};