#include "UFO.h"

UFO::UFO() : ShipBase()
{
	orientationSpeed = 0.0f;
	isShooting = false;
}

UFO::UFO(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead, float initorientationSpeed, bool initIsShooting) : ShipBase(initPosition, initOrientation, initSize, initTextureID, initDamage, initHealth, initMass, initAcceleration, initIsDead)
{
	setOrientationSpeed(initorientationSpeed);
	setIsShooting(initIsShooting);

}

UFO::~UFO()
{
	// Destructor logic if needed
}

// Getter methods
float UFO::getOrientationSpeed() const
{
	return this->orientationSpeed;
}
bool UFO::getIsShooting() const
{
	return this->isShooting;
}

// Setter methods
void UFO::setOrientationSpeed(float initOrientationAcceleration)
{
	this->orientationSpeed = initOrientationAcceleration;
}
void UFO::setIsShooting(bool initIsShooting)
{
	this->isShooting = initIsShooting;
}