#include "ProjectilesBase.h"

ProjectilesBase::ProjectilesBase() : GameObject2D()
{
	damage = 0;
	isDead = false;
	mass = 0.0f;
	acceleration = 0.0f;
}

ProjectilesBase::ProjectilesBase(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, bool initIsDead, float initMass, float initAcceleration) : GameObject2D(initPosition, initOrientation, initSize, initTextureID)
{
	setDamage(initDamage);
	setIsDead(initIsDead);
	setMass(initMass);
	setAcceleration(initAcceleration);
}

ProjectilesBase::~ProjectilesBase()
{
}

// Getter methods
int ProjectilesBase::getDamage() const
{
	return damage;
}
bool ProjectilesBase::getIsDead() const
{
	return isDead;
}
float ProjectilesBase::getMass() const
{
	return mass;
}
float ProjectilesBase::getAcceleration() const
{
	return acceleration;
}

// Setter methods
void ProjectilesBase::setDamage(int initDamage)
{
	damage = initDamage;
}
void ProjectilesBase::setIsDead(bool initIsDead)
{
	isDead = initIsDead;
}
void ProjectilesBase::setMass(float initMass)
{
	mass = initMass;
}
void ProjectilesBase::setAcceleration(float initAcceleration)
{
	acceleration = initAcceleration;
}