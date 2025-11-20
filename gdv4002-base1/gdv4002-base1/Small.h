#pragma once

#include "AstrodsBase.h"

class small : public AstrodsBase
{

public:
	small();
	small(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead);
	~small();
};

