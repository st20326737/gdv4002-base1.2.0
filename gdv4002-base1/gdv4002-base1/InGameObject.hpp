#pragma once
#include "Engine.h"

#ifndef INGAMEOBJECT_HPP_
#define INGAMEOBJECT_HPP_



const int MAX_NAME_LENGTH = 32;

class InGameObject
{
private:
	char name[MAX_NAME_LENGTH];
	float orientation;
	float momentum;
	int health;
	int damage;
	bool alive;

public:
	InGameObject();
	InGameObject(const char* objName, float objOrientation, float objMomentum, int objHealth, int objDamage);
	~InGameObject();

	const char* getName() const;
	float getOrientation() const;
	float getMomentum() const;
	int getHealth() const;
	int getDamage() const;
	bool getAlive() const;

	void setName(const char* objName);
	void setOrientation(float objOrientation);
	void setMomentum(float objMomentum);
	void setHealth(int objHealth);
	void setDamage(int objDamage);
	void setAlive(bool objAlive);
};




#endif // !INGAMEOBJECT_H_
