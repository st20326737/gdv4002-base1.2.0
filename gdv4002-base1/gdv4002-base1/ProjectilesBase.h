#pragma once
#include "Engine.h"
#include "GameObject2D.h"


class ProjectilesBase : public GameObject2D
{
private:
	int damage;
	bool isDead;
	float mass;
	float acceleration;

public:
	ProjectilesBase();
	ProjectilesBase(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, bool initIsDead, float initMass, float initAcceleration);
	~ProjectilesBase();

	// Getter methods
	int getDamage();
	bool getIsDead();
	float getMass();
	float getAcceleration();

	// Setter methods
	void setDamage(int initDamage);
	void setIsDead(bool initIsDead);
	void setMass(float initMass);
	void setAcceleration(float initAcceleration);

};

