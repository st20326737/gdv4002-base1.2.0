#include "InGameObject.hpp"
#include "Engine.h"
#include <cstring>



InGameObject::InGameObject()
{
	//orientation(objOrientation), momentum(objMomentum), health(objHealth), damage(objDamage), alive(true)
	orientation = 0.0f;
	momentum = 0.0f;
	health = 100;
	damage = 10;
	alive = true;
	name[0] = '\0'; // Initialize name to an empty string
}

InGameObject::InGameObject(const char* objName, float objOrientation, float objMomentum, int objHealth, int objDamage)
{
	setName(objName);
	setOrientation(objOrientation);
	setMomentum(objMomentum);
	setHealth(objHealth);
	setDamage(objDamage);
}
InGameObject::~InGameObject()
{
	// Destructor logic (if any) goes here
}
//getter methods
const char* InGameObject::getName() const
{
	return name;
}

float InGameObject::getOrientation() const
{
	return orientation;
}

float InGameObject::getMomentum() const
{
	return momentum;
}

int InGameObject::getHealth() const
{
	return health;
}

int InGameObject::getDamage() const
{
	return damage;
}

bool InGameObject::getAlive() const
{
	return alive;
}

//setter methods
void InGameObject::setName(const char* objName)
{
	strncpy_s(name, objName, MAX_NAME_LENGTH - 1);
	name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination
}

void InGameObject::setOrientation(float objOrientation)
{
	orientation = objOrientation;
}

void InGameObject::setMomentum(float objMomentum)
{
	momentum = objMomentum;
}

void InGameObject::setHealth(int objHealth)
{
	health = objHealth;
}

void InGameObject::setDamage(int objDamage)
{
	damage = objDamage;
}

void InGameObject::setAlive(bool objAlive)
{
	alive = objAlive;
}

