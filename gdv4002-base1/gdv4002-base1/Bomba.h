#pragma once
#include "ShipBase.h"

class Bomba : public ShipBase
{
private:
	float orientationSpeed;

public:
	Bomba();
	Bomba(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead, float initorientationSpeed);
	~Bomba();

	float getOrientationSpeed() const;

	void setOrientationSpeed(float initorientationSpeed);
};