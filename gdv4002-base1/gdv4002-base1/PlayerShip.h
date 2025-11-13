#pragma once
#include "ShipBase.h"

class PlayerShip : public ShipBase
{
private:
	float oriantationAcceleration;
	bool isShooting;

public:
	PlayerShip();
	PlayerShip(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead, float initOrientationAcceleration, bool initIsShooting);
	~PlayerShip();

	float getOrientationAcceleration() const;
	bool getIsShooting() const;

	void setOrientationAcceleration(float initOrientationAcceleration);
	void setIsShooting(bool initIsShooting);
};

