#include "PowerUpsBase.h"

PowerUpsBase::PowerUpsBase() : GameObject2D(), isDead(false), isCollected(false)
{
	setIsDead(false);
	setIsCollected(false);
}
PowerUpsBase::PowerUpsBase(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, bool initIsDead, bool initIsCollected) : GameObject2D(initPosition, initOrientation, initSize, initTextureID), isDead(initIsDead), isCollected(initIsCollected)
{
	setIsDead(initIsDead);
	setIsCollected(initIsCollected);
}
PowerUpsBase::~PowerUpsBase()
{
}

// Getter methods
bool PowerUpsBase::getIsDead() const
{
	return this->isDead;
}

bool PowerUpsBase::getIsCollected() const
{
	return this->isCollected;
}

// Setter methods
void PowerUpsBase::setIsDead(bool initIsDead)
{
	this->isDead = initIsDead;
}
void PowerUpsBase::setIsCollected(bool initIsCollected)
{
	this->isCollected = initIsCollected;
}