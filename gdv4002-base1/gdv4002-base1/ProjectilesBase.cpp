#include "ProjectilesBase.h"

ProjectilesBase::ProjectilesBase() : GameObject2D()
{
	damage = 0;
	isDead = false;
	mass = 0.0f;
	acceleration = 0.0f;
	dir = glm::vec2(0.0f, 0.0f);
}

ProjectilesBase::ProjectilesBase(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, bool initIsDead, float initMass, float initAcceleration, glm::vec2 initdir) : GameObject2D(initPosition, initOrientation, initSize, initTextureID)
{
	setDamage(initDamage);
	setIsDead(initIsDead);
	setMass(initMass);
	setAcceleration(initAcceleration);
	setDir(initdir);
}

ProjectilesBase::~ProjectilesBase()
{
}

// Getter methods
int ProjectilesBase::getDamage()
{
	return this->damage;
}
bool ProjectilesBase::getIsDead()
{
	return this->isDead;
}
float ProjectilesBase::getMass()
{
	return this->mass;
}
float ProjectilesBase::getAcceleration()
{
	return this->acceleration;
}
glm::vec2 ProjectilesBase::getDir()
{
	return this->dir;
}

// Setter methods
void ProjectilesBase::setDamage(int initDamage)
{
	this->damage = initDamage;
}
void ProjectilesBase::setIsDead(bool initIsDead)
{
	this->isDead = initIsDead;
}
void ProjectilesBase::setMass(float initMass)
{
	this->mass = initMass;
}
void ProjectilesBase::setAcceleration(float initAcceleration)
{
	this->acceleration = initAcceleration;
}
void ProjectilesBase::setDir(glm::vec2 initdir)
{
	this->dir = initdir;
}