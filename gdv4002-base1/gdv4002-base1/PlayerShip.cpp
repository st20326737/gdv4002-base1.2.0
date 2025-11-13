#include "PlayerShip.h"

PlayerShip::PlayerShip() : ShipBase()
{
	oriantationAcceleration = 0.0f;
	isShooting = false;
}

PlayerShip::PlayerShip(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead, float initOrientationAcceleration, bool initIsShooting) : ShipBase(initPosition, initOrientation, initSize, initTextureID, initDamage, initHealth, initMass, initAcceleration, initIsDead)
{
	setOrientationAcceleration(initOrientationAcceleration);
	setIsShooting(initIsShooting);

}

PlayerShip::~PlayerShip()
{
	// Destructor logic if needed
}

// Getter methods
float PlayerShip::getOrientationAcceleration() const
{
	return this->oriantationAcceleration;
}
bool PlayerShip::getIsShooting() const
{
	return this->isShooting;
}

// Setter methods
void PlayerShip::setOrientationAcceleration(float initOrientationAcceleration)
{
	this->oriantationAcceleration = initOrientationAcceleration;
}
void PlayerShip::setIsShooting(bool initIsShooting)
{
	this->isShooting = initIsShooting;
}