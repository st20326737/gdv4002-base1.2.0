#pragma once
#include "AstrodsBase.h"

class MidSized : public AstrodsBase
{

public:
	MidSized();
	MidSized(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead);
	~MidSized();
};

