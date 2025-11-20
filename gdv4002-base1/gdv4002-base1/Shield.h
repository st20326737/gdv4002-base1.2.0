#pragma once
#include "PowerUpsBase.h"

class Shield : public PowerUpsBase
{
private:
	bool shieldActive;

public:
	Shield();
	Shield(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, bool initIsDead, bool initIsCollected, bool initShieldActive);
	~Shield();

	// Getter method
	bool getShieldActive() const;

	// Setter method
	void setShieldActive(bool initShieldActive);

};

