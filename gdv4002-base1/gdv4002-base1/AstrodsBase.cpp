#include "AstrodsBase.h"

AstrodsBase::AstrodsBase() : GameObject2D()
{
	damage = 0;
	health = 0;
	mass = 0.0f;
	acceleration = 0.0f;
	isDead = true;
	orient = 0.0f;
	orientationAcceleration = 0.0f;
	velocity = 0.0f;
	dir = glm::vec2(0.0f, 0.0f);
}

AstrodsBase::AstrodsBase(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead, float orientationAcceleration, float initVelocity, glm::vec2 initdir) : GameObject2D(initPosition, initOrientation, initSize, initTextureID)
{

	setDamage(initDamage);
	setHealth(initHealth);
	setMass(initMass);
	setAcceleration(initAcceleration);
	setIsDead(initIsDead);
	setOrient(initOrientation);
	setOrientationAcceleration(orientationAcceleration);
	setVelocity(initVelocity);

}

AstrodsBase::~AstrodsBase()
{
	// Destructor logic if needed
}

// Getter methods
int AstrodsBase::getDamage() const
{
	return this->damage;
}
int AstrodsBase::getHealth() const
{
	return this->health;
}
float AstrodsBase::getMass() const
{
	return this->mass;
}
float AstrodsBase::getAcceleration() const
{
	return this->acceleration;
}
bool AstrodsBase::getIsDead() const
{
	return this->isDead;
}
float AstrodsBase::getOrient() const
{
	return this->orient;
}
float AstrodsBase::getOrientationAcceleration() const
{
	return this->orientationAcceleration;
}
float AstrodsBase::getVelocity()
{
	return this->velocity;
}
glm::vec2 AstrodsBase::getDir() const
{
	return this->dir;
}

// Setter methods
void AstrodsBase::setDamage(int initDamage)
{
	this->damage = initDamage;
}
void AstrodsBase::setHealth(int initHealth)
{
	this->health = initHealth;
}
void AstrodsBase::setMass(float initMass)
{
	this->mass = initMass;
}
void AstrodsBase::setAcceleration(float initAcceleration)
{
	this->acceleration = initAcceleration;
}
void AstrodsBase::setIsDead(bool initIsDead)
{
	this->isDead = initIsDead;
}
void AstrodsBase::setOrient(float orient)
{
	this->orient = orient;
}
void AstrodsBase::setOrientationAcceleration(float orientationAcceleration)
{
	this->orientationAcceleration = orientationAcceleration;
}
void AstrodsBase::setVelocity(float initVelocity)
{
	this->velocity = initVelocity;
}
void AstrodsBase::setDir(glm::vec2 initdir)
{
	this->dir = initdir;
}