#pragma once
#include "PowerUpsBase.h"

class Healing : public PowerUpsBase
{
private:
	int healAmount;

public:
	Healing();
	Healing(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, bool initIsDead, bool initIsCollected, int initHealAmount);
	~Healing();

	// Getter method
	int getHealAmount() const;

	// Setter method
	void setHealAmount(int initHealAmount);
};

