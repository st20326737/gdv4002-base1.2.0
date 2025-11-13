#pragma once

#include "ShipBase.h"

class UFO : public ShipBase
{
private:
	float orientationSpeed;
	bool isShooting;

public:
	UFO();
	UFO(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead, float initorientationSpeed, bool initIsShooting);
	~UFO();

	float orientationSpeed() const;
	bool getIsShooting() const;

	void orientationSpeed(float initorientationSpeed);
	void setIsShooting(bool initIsShooting);
};

