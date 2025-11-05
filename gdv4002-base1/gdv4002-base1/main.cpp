#include "Engine.h"
#include <cstdlib>
#include <ctime>

#include "InGameObject.hpp"

//global veriables
const float pi = 3.14159265359f;
const float maxSpeed = 200.0f;
const float maxTurn = 250.0f;
const float maxBulletSpeed = 250.0f;

static bool aPressed = false;//left
static bool dPressed = false;//right
static bool wPressed = false;//forward
static bool sPressed = false;//backward
static bool spacePressed = false;//shoot bullet
static bool flyingBullet = false;//bullet is flying

float turnVelocity = 0.0f;//- = left, + = right
float turnAcceleration = 5.0f; // degrees per second squared
float forwardVelocity = 0.0f;//- = back, + = front
float forwardAcceleration = 0.5f; // degrees per second squared
float dx = 0.0f;
float dy = 0.0f;
float shootCooldown = 0.2f;
float shoottimer = 1.0f;
float maxTurnAsteroid = 5.0f;
float maxSpeedAsteroid = 5.0f;



// Function prototypes
void myUpdateScene(GLFWwindow* window, double tDelta);
void myKeyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void keepOnScreen(float viewWidth, float viewHight, GameObject2D*);
void makePlayer();

void shootBullet(GameObject2D*, double tDelta);
void flyBullet(GameObjectCollection, double tDelta);
void bulletOffScreen(float, float);
void spawnAsteroid();
void turnAsteroid(double);

GameObject2D* player1;
GameObject2D* bullet;
GameObject2D* largeSize;
GameObject2D* midSize;
GameObject2D* smallSize;

int playerTextureID;
int bulletTextureID;
int largTextureID;
int midTextureID;
int smallTextureID;

int main(void)
{
	srand(time(0));



	// Pi constant
	const float pi = 3.14159265359f;

	//how many bullets can be on screen at once


	// Initialise the engine (create window, setup OpenGL backend)
	int initResult = engineInit("GDV4002 - Applied Maths for Games", 1024, 1024, 100);

	// If the engine initialisation failed report error and exit
	if (initResult != 0)
	{

		printf("Cannot setup game window!!!\n");
		return initResult; // exit if setup failed
	}

	//
	// Setup game scene objects here
	//
	playerTextureID = loadTexture("Resources\\Textures\\player1_ship.png", TextureProperties::NearestFilterTexture());
	bulletTextureID = loadTexture("Resources\\Textures\\bullet.png", TextureProperties::NearestFilterTexture());
	largTextureID = loadTexture("Resources\\Textures\\bullet.png", TextureProperties::NearestFilterTexture());
	midTextureID = loadTexture("Resources\\Textures\\bullet.png", TextureProperties::NearestFilterTexture());
	smallTextureID = loadTexture("Resources\\Textures\\bullet.png", TextureProperties::NearestFilterTexture());

	hideAxisLines();

	makePlayer();

	spawnAsteroid();

	setUpdateFunction(myUpdateScene);

	setKeyboardHandler(myKeyboardHandler);

	// Enter main loop - this handles update and render calls
	engineMainLoop();

	// When we quit (close window for example), clean up engine resources
	engineShutdown();

	// return success :)
	return 0;
}


void myUpdateScene(GLFWwindow* window, double tDelta)
{

	// Update game objects here
	keepOnScreen(getViewplaneWidth() / 2.0f, getViewplaneHeight() / 2.0f, player1);

	turnAsteroid((float)tDelta);

	// Update player rotation based on key presses
	float thetaVelocity; // radians per second

	shoottimer += (float)tDelta;

	if (aPressed)
	{
		turnVelocity += turnAcceleration;
	}
	if (dPressed)
	{
		turnVelocity -= turnAcceleration;
	}

	if (fabs(turnVelocity) > maxTurn)
	{
		turnVelocity = (turnVelocity / fabs(turnVelocity)) * maxTurn;
	}

	thetaVelocity = (pi / 180.0f) * turnVelocity;

	player1->orientation += thetaVelocity * (float)tDelta;

	// Update player speed based on key presses
	if (shoottimer >= shootCooldown)
	{
		if (spacePressed)
		{
			shootBullet(player1, (float)tDelta);
			shoottimer = 0.0f;
			flyingBullet = true;
		}
	}

	if (wPressed)
	{
		forwardVelocity += forwardAcceleration;
	}
	if (sPressed)
	{
		forwardVelocity -= forwardAcceleration;
	}

	if (fabs(forwardVelocity) > maxSpeed)
	{
		forwardVelocity = (forwardVelocity / fabs(forwardVelocity)) * maxSpeed;//scale to max speed with sign
	}

	dx = forwardVelocity * cos(player1->orientation) * (float)tDelta;
	dy = forwardVelocity * sin(player1->orientation) * (float)tDelta;

	player1->position.x += dx;
	player1->position.y += dy;



	GameObjectCollection bullets = getObjectCollection("bullet");
	flyBullet(bullets, (float)tDelta);

	bulletOffScreen(getViewplaneWidth() / 2.0f, getViewplaneHeight() / 2.0f);



}


void myKeyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check if a key is pressed
	if (action == GLFW_PRESS)
	{

		// check which key was pressed...
		switch (key)
		{
		case GLFW_KEY_A:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			aPressed = true;
			break;

		case GLFW_KEY_D:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			dPressed = true;
			break;

		case GLFW_KEY_W:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			wPressed = true;
			break;

		case GLFW_KEY_S:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			sPressed = true;
			break;

		case GLFW_KEY_SPACE:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			spacePressed = true;
			break;
		default:
		{
		}
		}
	}
	// If not check a key has been released
	else if (action == GLFW_RELEASE)
	{

		// handle key release events
		switch (key)
		{
		case GLFW_KEY_A:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			aPressed = false;
			break;

		case GLFW_KEY_D:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			dPressed = false;
			break;

		case GLFW_KEY_W:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			wPressed = false;
			break;

		case GLFW_KEY_S:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			sPressed = false;
			break;

		case GLFW_KEY_SPACE:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			spacePressed = false;
			break;
		default:
		{
		}
		}
	}
}

void keepOnScreen(float viewWidth, float viewHight, GameObject2D* player1)
{
	float width = viewWidth + 2.0f;
	float height = viewHight + 2.0f;

	if (player1->position.x < width && player1->position.x > -width)
	{
		//do nothing
	}
	else
	{
		player1->position.x *= -1.0f;
	}


	if (player1->position.y < height && player1->position.y > -height)
	{
		//do nothing
	}
	else
	{
		player1->position.y *= -1.0f;
	}
}

void makePlayer()
{
	
	player1 = new GameObject2D(glm::vec2(0.0f, 0.0f), glm::radians(90.0f), glm::vec2(5.0f, 5.0f), playerTextureID);
	addObject("player", player1);

	InGameObject* ship = new InGameObject("player1_ship", 0.0f, glm::radians(90.0f), 100, 999);

}



void shootBullet(GameObject2D* player1, double tDelta)
{
	
	bullet = new GameObject2D(glm::vec2(player1->position.x, player1->position.y), player1->orientation, glm::vec2(2.5f, 2.5f), bulletTextureID);
	addObject("bullet", bullet);

	glm::vec2 temp = glm::vec2(player1->position.x, player1->position.y);

	InGameObject* plasma = new InGameObject("bullet", 0.0f, glm::radians(90.0f), 100, 999);

}
void flyBullet(GameObjectCollection bullet, double tDelta)
{
	for (int x = 0; x < bullet.objectCount; x++)
	{
		GameObject2D* thisBullet = bullet.objectArray[x];
		if (thisBullet == nullptr)
		{
			continue;
		}

		thisBullet->position.x += maxBulletSpeed * cos(thisBullet->orientation) * (float)tDelta;
		thisBullet->position.y += maxBulletSpeed * sin(thisBullet->orientation) * (float)tDelta;
	}

}
void bulletOffScreen(float viewWidth, float viewHight)
{
	float width = viewWidth + 2.0f;
	float height = viewHight + 2.0f;

	GameObjectCollection bullet = getObjectCollection("bullet");

	for (int x = 0; x < bullet.objectCount; x++)
	{
		GameObject2D* thisBullet = bullet.objectArray[x];

		if (thisBullet == nullptr)
		{
			continue;
		}

		if (thisBullet->position.x < width && thisBullet->position.x > -width)
		{
			if (thisBullet->position.y < height && thisBullet->position.y > -height)
			{
				//do nothing
			}
			else
			{
				deleteObject(thisBullet);
				return;

			}
		}
		else
		{
			deleteObject(thisBullet);
			return;


		}
	}
}

void spawnAsteroid()
{
	int type, amount;
	amount = rand() % 10;
	amount += 1;
	while (amount > 0)
	{
		printf("%d\n", amount);
		type = rand() % 3;
		if (type == 0)
		{
			addObject("big", glm::vec2((float)(rand() % 100 - 50), (float)(rand() % 100 - 50)), glm::radians(0.0f), glm::vec2(20.0f, 20.0f), "Resources\\Textures\\larg.png", TextureProperties::NearestFilterTexture());
			amount--;
		}
		else if (type == 1)
		{
			addObject("mid", glm::vec2((float)(rand() % 100 - 50), (float)(rand() % 100 - 50)), glm::radians(0.0f), glm::vec2(10.0f, 10.0f), "Resources\\Textures\\mid.png", TextureProperties::NearestFilterTexture());
			amount--;
		}
		else
		{
			addObject("small", glm::vec2((float)(rand() % 100 - 50), (float)(rand() % 100 - 50)), glm::radians(0.0f), glm::vec2(5.0f, 5.0f), "Resources\\Textures\\small.png", TextureProperties::NearestFilterTexture());
			amount--;
		}
	}

}

void turnAsteroid(double tDelta)
{
	GameObjectCollection big = getObjectCollection("big");
	GameObjectCollection mid = getObjectCollection("mid");
	GameObjectCollection small = getObjectCollection("small");

	if (big.objectCount == 0 && mid.objectCount == 0 && small.objectCount == 0)
	{
		return;
	}
	if (big.objectCount != 0)
	{
		for (int x = 0; x < big.objectCount; x++)
		{
			GameObject2D* thisBig = big.objectArray[x];
			if (thisBig == nullptr)
			{
				continue;
			}
			float thetaVelocity = (pi / 180.0f) * maxTurnAsteroid;
			thisBig->orientation += thetaVelocity * (float)tDelta;
		}
	}
	if (mid.objectCount != 0)
	{
		for (int x = 0; x < mid.objectCount; x++)
		{
			GameObject2D* thisMid = mid.objectArray[x];
			if (thisMid == nullptr)
			{
				continue;
			}
			float thetaVelocity = (pi / 180.0f) * maxTurnAsteroid * 5;
			thisMid->orientation += thetaVelocity * (float)tDelta;
		}
	}
	if (small.objectCount != 0)
	{
		for (int x = 0; x < small.objectCount; x++)
		{
			GameObject2D* thisSmall = small.objectArray[x];
			if (thisSmall == nullptr)
			{
				continue;
			}
			float thetaVelocity = (pi / 180.0f) * maxTurnAsteroid * 10;
			thisSmall->orientation += thetaVelocity * (float)tDelta;
		}
	}
}

void moveAsteroid(double tDelta)
{
	GameObjectCollection big = getObjectCollection("big");
	GameObjectCollection mid = getObjectCollection("mid");
	GameObjectCollection small = getObjectCollection("small");
	float dx, dy;

	if (big.objectCount == 0 && mid.objectCount == 0 && small.objectCount == 0)
	{
		return;
	}
	if (big.objectCount != 0)
	{
		for (int x = 0; x < big.objectCount; x++)
		{
			GameObject2D* thisBig = big.objectArray[x];
			if (thisBig == nullptr)
			{
				continue;
			}

			dx = maxSpeedAsteroid * cos(thisBig->orientation) * (float)tDelta;// these need to be changed to a fixed angle, for now just using orientation
			dy = maxSpeedAsteroid * sin(thisBig->orientation) * (float)tDelta;

			thisBig->position.x += dx;
			thisBig->position.y += dy;

		}
	}
	if (mid.objectCount != 0)
	{
		for (int x = 0; x < mid.objectCount; x++)
		{
			GameObject2D* thisMid = mid.objectArray[x];
			if (thisMid == nullptr)
			{
				continue;
			}
			float thetaVelocity = (pi / 180.0f) * maxTurnAsteroid * 5;
			thisMid->orientation += thetaVelocity * (float)tDelta;
		}
	}
	if (small.objectCount != 0)
	{
		for (int x = 0; x < small.objectCount; x++)
		{
			GameObject2D* thisSmall = small.objectArray[x];
			if (thisSmall == nullptr)
			{
				continue;
			}
			float thetaVelocity = (pi / 180.0f) * maxTurnAsteroid * 10;
			thisSmall->orientation += thetaVelocity * (float)tDelta;
		}
	}
}
