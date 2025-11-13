#include "Bomba.h"

Bomba::Bomba() : ShipBase()
{
	orientationSpeed = 0.0f;
}

Bomba::Bomba(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead, float initorientationSpeed) : ShipBase(initPosition, initOrientation, initSize, initTextureID, initDamage, initHealth, initMass, initAcceleration, initIsDead)
{
	setOrientationSpeed(initorientationSpeed);

}

Bomba::~Bomba()
{
	// Destructor logic if needed
}

// Getter methods
float Bomba::getOrientationSpeed() const
{
	return this->orientationSpeed;
}
// Setter methods
void Bomba::setOrientationSpeed(float initOrientationAcceleration)
{
	this->orientationSpeed = initOrientationAcceleration;
}