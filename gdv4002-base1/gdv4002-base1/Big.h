#pragma once
#include "AstrodsBase.h"

class Big : public AstrodsBase
{

public:
	Big();
	Big(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead);
	~Big();
};

