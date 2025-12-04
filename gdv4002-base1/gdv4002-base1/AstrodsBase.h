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
	float orient;
	float orientationAcceleration;
	float velocity;

public:
	AstrodsBase();
	AstrodsBase(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead, float orientationAcceleration, float initVelocity);
	~AstrodsBase();

	// Getter methods
	int getDamage() const;
	int getHealth() const;
	float getMass() const;
	float getAcceleration() const;
	bool getIsDead() const;
	float getOrient() const;
	float getOrientationAcceleration() const;
	float getVelocity();

	// Setter methods
	void setDamage(int initDamage);
	void setHealth(int initHealth);
	void setMass(float initMass);
	void setAcceleration(float initAcceleration);
	void setIsDead(bool initAlive);
	void setOrient(float orient);
	void setOrientationAcceleration(float orientationAcceleration);
	void setVelocity(float initVelocity);

};

