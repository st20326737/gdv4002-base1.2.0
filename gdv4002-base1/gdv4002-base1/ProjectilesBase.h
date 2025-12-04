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
	glm::vec2 dir;

public:
	ProjectilesBase();
	ProjectilesBase(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, bool initIsDead, float initMass, float initAcceleration, glm::vec2 dir);
	~ProjectilesBase();

	// Getter methods
	int getDamage();
	bool getIsDead();
	float getMass();
	float getAcceleration();
	glm::vec2 getDir();

	// Setter methods
	void setDamage(int initDamage);
	void setIsDead(bool initIsDead);
	void setMass(float initMass);
	void setAcceleration(float initAcceleration);
	void setDir(glm::vec2 dir);

};

