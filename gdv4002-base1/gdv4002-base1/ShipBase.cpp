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
	/*
	damage = initDamage;
	health = initHealth;
	mass = initMass;
	acceleration = initAcceleration;
	isDead = initIsDead;
	*/
	

}