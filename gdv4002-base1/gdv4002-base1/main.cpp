#include "Engine.h"
#include <cstdlib>
#include <ctime>

#include "Bomba.h"
#include "Healing.h"
#include "PlayerShip.h"
#include "Shield.h"
#include "UFO.h"
#include "AstrodsBase.h"
#include "ProjectilesBase.h"

using std::endl;
using std::cout;

//global veriables
const float pi = 3.14159265359f;
const float maxSpeed = 200.0f;
const float maxTurn = 250.0f;
const float maxBulletSpeed = 250.0f;

float thetaVelocity; // radians per second

static bool aPressed = false;//left
static bool dPressed = false;//right
static bool wPressed = false;//forward
static bool sPressed = false;//backward
static bool spacePressed = false;//shoot bullet
static bool shiftPressed = false;//break
static bool flyingBullet = false;//bullet is flying

float turnVelocity = 0.0f;//- = left, + = right
float turnAcceleration = 5.0f; // degrees per second squared
float forwardVelocity = 0.0f;//- = back, + = front
float forwardAcceleration = 0.5f; // degrees per second squared
float dx = 0.0f;
float dy = 0.0f;
float shootCooldown = 0.05f;
float shoottimer = 1.0f;
float maxTurnAsteroid = 5.0f;
float maxSpeedAsteroid = 5.0f;
int level = 0;
int difficultyPoints = 100;
int astrodsThatIsDead = 0;

//arrays
AstrodsBase* smallSize[40];
AstrodsBase* midSize[20];
AstrodsBase* bigSize[10];

ProjectilesBase* bulletArray[15];
ProjectilesBase* enemyBulletArray[50];
ProjectilesBase* missileArray[2];


// Function prototypes
void myUpdateScene(GLFWwindow* window, double tDelta);
void myKeyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void keepOnScreen(float viewWidth, float viewHight, PlayerShip*);
void makePlayer();

void shootBullet(PlayerShip*, double tDelta);
void flyBullet(double tDelta);
void bulletOffScreen(float, float);
void spawnAsteroid();
void setUpArrays();
void setUpLevel();

void myRenderScene(GLFWwindow* window);
void movementAsteroids(double tDelta);
void keepAsteroidsOnScreen(float viewWidth, float viewHight);
void orientationAsteroids(double tDelta);

//player ship
PlayerShip* playerShip;

//texture IDs
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
	largTextureID = loadTexture("Resources\\Textures\\larg.png", TextureProperties::NearestFilterTexture());
	midTextureID = loadTexture("Resources\\Textures\\mid.png", TextureProperties::NearestFilterTexture());
	smallTextureID = loadTexture("Resources\\Textures\\small.png", TextureProperties::NearestFilterTexture());

	// set up the arrays
	setUpArrays();

	//hide axis lines
	hideAxisLines();

	//make player ship
	makePlayer();

	// Register update function with engine
	setUpdateFunction(myUpdateScene);

	// Register keyboard handler with engine
	setKeyboardHandler(myKeyboardHandler);

	setRenderFunction(myRenderScene);

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
	keepOnScreen(getViewplaneWidth() / 2.0f, getViewplaneHeight() / 2.0f, playerShip);
	keepAsteroidsOnScreen(getViewplaneWidth() / 2.0f, getViewplaneHeight() / 2.0f);

	//update level
	setUpLevel();

	//move asteroids
	movementAsteroids((double)tDelta);
	orientationAsteroids((double)tDelta);

	//shoot
	shoottimer += (float)tDelta;

	if (shoottimer >= shootCooldown)
	{
		if (spacePressed)
		{
			shootBullet(playerShip, (float)tDelta);
			shoottimer = 0.0f;
		}
	}

	flyBullet(tDelta);

	bulletOffScreen(getViewplaneWidth() / 2.0f, getViewplaneHeight() / 2.0f);

	//turn
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

	playerShip->orientation += thetaVelocity * (float)tDelta;

	//glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead, float initOrientationAcceleration, bool initIsShooting
	//accelerate
	if (wPressed)
	{
		forwardVelocity += forwardAcceleration;
	}
	if (sPressed)
	{
		forwardVelocity -= forwardAcceleration;
	}
	if (shiftPressed)
	{
		if (forwardVelocity > 0)
		{
			forwardVelocity -= forwardAcceleration;
		}
		else if (forwardVelocity < 0)
		{
			forwardVelocity += forwardAcceleration;
		}
	}

	if (fabs(forwardVelocity) > maxSpeed)
	{
		forwardVelocity = (forwardVelocity / fabs(forwardVelocity)) * maxSpeed;//scale to max speed with sign
	}

	dx = forwardVelocity * cos(playerShip->orientation) * (float)tDelta;
	dy = forwardVelocity * sin(playerShip->orientation) * (float)tDelta;

	playerShip->position.x += dx;
	playerShip->position.y += dy;

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
		case GLFW_KEY_LEFT:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			aPressed = true;
			break;

		case GLFW_KEY_D:
		case GLFW_KEY_RIGHT:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			dPressed = true;
			break;

		case GLFW_KEY_W:
		case GLFW_KEY_UP:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			wPressed = true;
			break;

		case GLFW_KEY_S:
		case GLFW_KEY_DOWN:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			sPressed = true;
			break;

		case GLFW_KEY_SPACE:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			spacePressed = true;
			break;

		case GLFW_KEY_LEFT_SHIFT:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			shiftPressed = true;
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
		case GLFW_KEY_LEFT:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			aPressed = false;
			break;

		case GLFW_KEY_D:
		case GLFW_KEY_RIGHT:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			dPressed = false;
			break;

		case GLFW_KEY_W:
		case GLFW_KEY_UP:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			wPressed = false;
			break;

		case GLFW_KEY_S:	
		case GLFW_KEY_DOWN:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			sPressed = false;
			break;

		case GLFW_KEY_SPACE:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			spacePressed = false;
			break;

		case GLFW_KEY_LEFT_SHIFT:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			shiftPressed = false;
			break;
		default:
		{
		}
		}
	}
}

void setUpArrays()
{
	//asteroid arrays
	//glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead
	for (int i = 0; i < 40; i++)
	{
		smallSize[i] = new AstrodsBase(glm::vec2(200.0f, 0.0f), glm::radians(90.0f), glm::vec2(5.0f, 5.0f), smallTextureID, 5, 10, 10.0f, 5.0f, true);
	}
	for (int j = 0; j < 20; j++)
	{
		midSize[j] = new AstrodsBase(glm::vec2(200.0f, 0.0f), glm::radians(90.0f), glm::vec2(10.0f, 10.0f), midTextureID, 10, 20, 20.0f, 2.0f, true);
	}
	for (int z = 0; z < 10; z++)
	{
		bigSize[z] = new AstrodsBase(glm::vec2(200.0f, 0.0f), glm::radians(90.0f), glm::vec2(20.0f, 20.0f), largTextureID, 20, 40, 40.0f, 0.5f, true);
	}

	//projectile arrays
	//glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, bool initIsDead, float initMass, float initAcceleration
	for (int a = 0; a < 15; a++)
	{
		bulletArray[a] = new ProjectilesBase(glm::vec2(0.0f, 200.0f), glm::radians(90.0f), glm::vec2(2.5f, 2.5f), bulletTextureID, 10, true, 1.0f, 250.0f);
		cout << "bullet array set up at index " << a << endl;
		cout << a << ": " << bulletArray[a]->getIsDead() << endl;
	}
	for (int b = 0; b < 50; b++)
	{
		enemyBulletArray[b] = new ProjectilesBase(glm::vec2(0.0f, 200.0f), glm::radians(90.0f), glm::vec2(2.5f, 2.5f), bulletTextureID, 10, true, 1.0f, 250.0f);
	}
	for (int c = 0; c < 2; c++)
	{
		missileArray[c] = new ProjectilesBase(glm::vec2(0.0f, 200.0f), glm::radians(90.0f), glm::vec2(2.5f, 2.5f), bulletTextureID, 50, true, 10.0f, 50.0f);
	}
}


void keepOnScreen(float viewWidth, float viewHight, PlayerShip* player1)
{
	float width = viewWidth + 1.0f;
	float height = viewHight + 1.0f;

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

void keepAsteroidsOnScreen(float viewWidth, float viewHight)
{
	float width = viewWidth;
	float height = viewHight;
	for (int i = 0; i < 40; i++)
	{
		if (smallSize[i]->getIsDead() == false)//5
		{
			if (smallSize[i]->position.x - 2 < width && smallSize[i]->position.x + 2 > -width)
			{
				//do nothing
			}
			else
			{
				smallSize[i]->position.x *= -1.0f;
			}

			if (smallSize[i]->position.y - 2 < height && smallSize[i]->position.y + 2 > -height)
			{
				//do nothing
			}
			else
			{
				smallSize[i]->position.y *= -1.0f;
			}
		}
	}
	for (int j = 0; j < 20; j++)
	{
		if (midSize[j]->getIsDead() == false)//10
		{
			if (midSize[j]->position.x - 5 < width && midSize[j]->position.x + 5 > -width)
			{
				//do nothing
			}
			else
			{
				midSize[j]->position.x *= -1.0f;
			}
			if (midSize[j]->position.y - 5 < height && midSize[j]->position.y + 5> -height)
			{
				//do nothing
			}
			else
			{
				midSize[j]->position.y *= -1.0f;
			}
		}
	}
	for (int z = 0; z < 10; z++)
	{
		if (bigSize[z]->getIsDead() == false)//20
		{
			if (bigSize[z]->position.x - 10 < width && bigSize[z]->position.x + 10 > -width)
			{
				//do nothing
			}
			else
			{
				bigSize[z]->position.x *= -1.0f;
			}
			if (bigSize[z]->position.y - 10 < height && bigSize[z]->position.y +10 > -height)
			{
				//do nothing
			}
			else
			{
				bigSize[z]->position.y *= -1.0f;
			}
		}
	}
}


void orientationAsteroids(double tDelta)
{
	for (int i = 0; i < 40; i++)
	{
		if (smallSize[i]->getIsDead() == false)
		{
			smallSize[i]->orientation += maxTurnAsteroid * (float)tDelta;
		}
	}
	for (int j = 0; j < 20; j++)
	{
		if (midSize[j]->getIsDead() == false)
		{
			midSize[j]->orientation += maxTurnAsteroid * (float)tDelta;
		}
	}
	for (int z = 0; z < 10; z++)
	{
		if (bigSize[z]->getIsDead() == false)
		{
			bigSize[z]->orientation += maxTurnAsteroid * (float)tDelta;
		}
	}
}

void setUpLevel()
{
	astrodsThatIsDead = 0;
	
	for (int i = 0; i < 40; i++)
	{
		if (smallSize[i]->getIsDead() == true) 
		{
			astrodsThatIsDead += 1;
		}
	}
	for (int j = 0; j < 20; j++)
	{
		if (midSize[j]->getIsDead() == true)
		{
			astrodsThatIsDead += 1;
		}
	}
	for (int z = 0; z < 10; z++)
	{
		if (bigSize[z]->getIsDead() == true)
		{
			astrodsThatIsDead += 1;
		}
	}

	if (astrodsThatIsDead >= 70)
	{
		level += 1;
		cout << "Level up! Current level: " << level << endl;
		spawnAsteroid();
	}

}

void spawnAsteroid()
{
	int type;
	difficultyPoints *= level;
	while (difficultyPoints > 0)
	{
		printf("%d\n", difficultyPoints);
		type = rand() % 3;
		if (type == 0)
		{
			//spawn big asteroid
			for (int z = 0; z < 10; z++)
			{
				if(bigSize[z]->getIsDead())
				{
					bigSize[z]->setIsDead(false);
					difficultyPoints -= bigSize[z]->getHealth();
					bigSize[z]->position = glm::vec2((float)(rand() % 100 - 50), (float)(rand() % 100 - 50));
					bigSize[z]->orientation = glm::radians((float)(rand() % 360));
					break;
				}
			}
			
		}
		else if (type == 1)
		{
			//spawn mid asteroid
			for (int j = 0; j < 20; j++)
			{
				if(midSize[j]->getIsDead())
				{
					midSize[j]->setIsDead(false);
					difficultyPoints -= midSize[j]->getHealth();
					midSize[j]->position = glm::vec2((float)(rand() % 100 - 50), (float)(rand() % 100 - 50));
					midSize[j]->orientation = glm::radians((float)(rand() % 360));
					break;
				}
			}
		}
		else
		{
			//spawn small asteroid
			for (int i = 0; i < 40; i++)
			{
				if(smallSize[i]->getIsDead())
				{
					smallSize[i]->setIsDead(false);
					difficultyPoints -= smallSize[i]->getHealth();
					smallSize[i]->position = glm::vec2((float)(rand() % 100 - 50), (float)(rand() % 100 - 50));
					smallSize[i]->orientation = glm::radians((float)(rand() % 360));
					break;
				}
			}
		}
	}
}

void movementAsteroids(double tDelta)
{
	for (int i = 0; i < 40; i++)
	{
		if (smallSize[i]->getIsDead() == false)
		{
			dx = smallSize[i]->getAcceleration() * cos(smallSize[i]->orientation) * (float)tDelta;
			dy = smallSize[i]->getAcceleration() * sin(smallSize[i]->orientation) * (float)tDelta;

			smallSize[i]->position.x += dx;
			smallSize[i]->position.y += dy;
		}
	}
	for (int j = 0; j < 20; j++)
	{
		if (midSize[j]->getIsDead() == false)
		{
			dx = midSize[j]->getAcceleration() * cos(midSize[j]->orientation) * (float)tDelta;
			dy = midSize[j]->getAcceleration() * sin(midSize[j]->orientation) * (float)tDelta;

			midSize[j]->position.x += dx;
			midSize[j]->position.y += dy;
		}
	}
	for (int z = 0; z < 10; z++)
	{
		if (bigSize[z]->getIsDead() == false)
		{
			dx = bigSize[z]->getAcceleration() * cos(bigSize[z]->orientation) * (float)tDelta;
			dy = bigSize[z]->getAcceleration() * sin(bigSize[z]->orientation) * (float)tDelta;

			bigSize[z]->position.x += dx;
			bigSize[z]->position.y += dy;
		}
	}
}

void makePlayer()
{
	
	//glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead, float initOrientationAcceleration, bool initIsShooting
	playerShip = new PlayerShip(glm::vec2(0.0f, 0.0f), glm::radians(90.0f), glm::vec2(5.0f, 5.0f), playerTextureID, 100, 100, 25.0f, 0.5f, false, 5.0f, false);


}



void shootBullet(PlayerShip* playerShip, double tDelta)
{
	for (int i = 0; i < 15; i++)
	{
		if (bulletArray[i]->getIsDead())
		{
			bulletArray[i]->setIsDead(false);
			bulletArray[i]->position = playerShip->position;
			bulletArray[i]->orientation = playerShip->orientation;
			break;
		}
	}

}

void flyBullet(double tDelta)
{
	for (int i = 0; i < 15; i++)
	{
		if (!(bulletArray[i]->getIsDead()))
		{
			float dx = bulletArray[i]->getAcceleration() * cos(bulletArray[i]->orientation) * (float)tDelta;
			float dy = bulletArray[i]->getAcceleration() * sin(bulletArray[i]->orientation) * (float)tDelta;
			bulletArray[i]->position.x += dx;
			bulletArray[i]->position.y += dy;
			cout << "flying bullet from index " << i << endl;
		}
	}

}
void bulletOffScreen(float viewWidth, float viewHight)
{
	float width = viewWidth + 2.0f;
	float height = viewHight + 2.0f;

	for (int i = 0; i < 15; i++)
	{
		if (!(bulletArray[i]->getIsDead()))
		{
			if (bulletArray[i]->position.x < width && bulletArray[i]->position.x > -width && bulletArray[i]->position.y < height && bulletArray[i]->position.y > -height && bulletArray[i]->getIsDead() == false)
			{
				//do nothing
			}
			else
			{
				bulletArray[i]->setIsDead(true);
				bulletArray[i]->position = glm::vec2(0.0f, 200.0f);
				cout << "bullet despawns " << i << endl;
			}
		}
	}
	
}

void myRenderScene(GLFWwindow* window)
{
	
	// Render player ship
	playerShip->render();

	// Render asteroids
	for (int i = 0; i < 40; i++)
	{
		if (smallSize[i]->getIsDead())
		{
			continue;
		}
		smallSize[i]->render();
	}
	for (int j = 0; j < 20; j++)
	{
		if (midSize[j]->getIsDead())
		{
			continue;
		}
		midSize[j]->render();
	}
	for (int z = 0; z < 10; z++)
	{
		if (bigSize[z]->getIsDead())
		{
			continue;
		}
		bigSize[z]->render();
	}

	// Render bullets
	for (int a = 0; a < 15; a++)
	{
		if (bulletArray[a]->getIsDead())
		{
			continue;
		}
		bulletArray[a]->render();
	}
	for (int b = 0; b < 50; b++)
	{
		if (enemyBulletArray[b]->getIsDead())
		{
			continue;
		}
		enemyBulletArray[b]->render();
	}
	for (int c = 0; c < 2; c++)
	{
		if (missileArray[c]->getIsDead())
		{
			continue;
		}
		missileArray[c]->render();
	}
}



