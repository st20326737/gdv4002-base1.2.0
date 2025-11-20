#include "Shield.h"

Shield::Shield() : PowerUpsBase()
{
	shieldActive = false;
}
Shield::Shield(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, bool initIsDead, bool initIsCollected, bool initShieldActive) : PowerUpsBase(initPosition, initOrientation, initSize, initTextureID, initIsDead, initIsCollected)
{
	setShieldActive(initShieldActive);
}
Shield::~Shield()
{
}

// Getter method
bool Shield::getShieldActive() const
{
	return shieldActive;
}

// Setter method
void Shield::setShieldActive(bool initShieldActive)
{
	shieldActive = initShieldActive;
}