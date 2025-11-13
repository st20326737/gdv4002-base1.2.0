#include "ShipBase.h"

ShipBase::ShipBase() : GameObject2D() 
{
	damage = 0;
	health = 0;
	mass = 0.0f;
	acceleration = 0.0f;
	isDead = true;
}

ShipBase::ShipBase(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead) : GameObject2D(initPosition, initOrientation, initSize, initTextureID)
{
	
	setDamage(initDamage);
	setHealth(initHealth);
	setMass(initMass);
	setAcceleration(initAcceleration);
	setIsDead(initIsDead);

}

ShipBase::~ShipBase() 
{
	// Destructor logic if needed
}

// Getter methods
int ShipBase::getDamage() const 
{
	return this->damage;
}
int ShipBase::getHealth() const 
{
	return this->health;
}
float ShipBase::getMass() const 
{
	return this->mass;
}
float ShipBase::getAcceleration() const 
{
	return this->acceleration;
}
bool ShipBase::getIsDead() const 
{
	return this->isDead;
}

// Setter methods
void ShipBase::setDamage(int initDamage) 
{
	this->damage = initDamage;
}
void ShipBase::setHealth(int initHealth) 
{
	this->health = initHealth;
}
void ShipBase::setMass(float initMass) 
{
	this->mass = initMass;
}
void ShipBase::setAcceleration(float initAcceleration) 
{
	this->acceleration = initAcceleration;
}
void ShipBase::setIsDead(bool initIsDead) 
{
	this->isDead = initIsDead;
}
