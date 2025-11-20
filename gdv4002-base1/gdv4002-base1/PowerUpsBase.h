#pragma once
#include "Engine.h"
#include "GameObject2D.h"

class PowerUpsBase : public GameObject2D
{
private:
	bool isDead;
	bool isCollected;

public:
	PowerUpsBase();
	PowerUpsBase(glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, bool initIsDead, bool initIsCollected);
	~PowerUpsBase();

	// Getter methods
	bool getIsDead() const;
	bool getIsCollected() const;

	// Setter methods
	void setIsDead(bool initIsDead);
	void setIsCollected(bool initIsCollected);
};

