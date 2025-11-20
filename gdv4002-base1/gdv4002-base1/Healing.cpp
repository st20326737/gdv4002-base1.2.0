#include "Healing.h"

Healing::Healing() : PowerUpsBase()
{
	healAmount = 0;
}
Healing::Healing(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, bool initIsDead, bool initIsCollected, int initHealAmount) : PowerUpsBase(initPosition, initOrientation, initSize, initTextureID, initIsDead, initIsCollected)
{
	setHealAmount(initHealAmount);
}
Healing::~Healing()
{
}

// Getter method
int Healing::getHealAmount() const
{
	return healAmount;
}

// Setter method
void Healing::setHealAmount(int initHealAmount)
{
	healAmount = initHealAmount;
}