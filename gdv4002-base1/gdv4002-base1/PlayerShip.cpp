#include "PlayerShip.h"

PlayerShip::PlayerShip() : ShipBase()
{
	
}

PlayerShip::PlayerShip(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead, float initOrientationAcceleration, bool initIsShooting) : ShipBase(initPosition, initOrientation, initSize, initTextureID, initDamage, initHealth, initMass, initAcceleration, initIsDead)
{

	

}

PlayerShip::~PlayerShip()
{
	// Destructor logic if needed
}