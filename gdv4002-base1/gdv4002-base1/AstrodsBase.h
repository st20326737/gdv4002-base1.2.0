#pragma once

#include "Engine.h"
#include "GameObject2D.h"


class AstrodsBase : public GameObject2D
{
private:
	int damage;
	int health;
	float mass;
	float acceleration;
	bool isDead;

public:
	AstrodsBase();
	AstrodsBase(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead);
	~AstrodsBase();

	// Getter methods
	int getDamage() const;
	int getHealth() const;
	float getMass() const;
	float getAcceleration() const;
	bool getIsDead() const;

	// Setter methods
	void setDamage(int initDamage);
	void setHealth(int initHealth);
	void setMass(float initMass);
	void setAcceleration(float initAcceleration);
	void setIsDead(bool initAlive);

};

