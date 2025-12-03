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
AstrodsBase* smallSize[400];
AstrodsBase* midSize[200];
AstrodsBase* bigSize[100];

ProjectilesBase* bulletArray[15];
ProjectilesBase* enemyBulletArray[50];
ProjectilesBase* missileArray[2];

UFO* alienUFOs[10];




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

void bulletHit(double tDelta);

//player ship
PlayerShip* playerShip;

//texture IDs
int playerTextureID;
int UFOTextureID;
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
	UFOTextureID = loadTexture("Resources\\Textures\\UFO.png", TextureProperties::NearestFilterTexture());
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

	//check for hits
	bulletHit((double)tDelta);

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

void bulletHit(double tDelta)
{
	//mass * acceleration = force
	//therefore the transfur of force to acceleration is force / mass = acceleration

	//oriantation = oriantation + bullet oriantation * tDelta

	float force;

	//handle hit
	for (int i = 0; i < 15; i++)//bulletArray
	{
		for (int j = 0; j < 400; j++)//smallSize
		{
			if (bulletArray[i]->getIsDead() == false && smallSize[j]->getIsDead() == false)
			{
				float distance = glm::distance(bulletArray[i]->position, smallSize[j]->position);
				if (distance < 5.0f)//5 is radius of small asteroid + radius of bullet
				{
					//hit detected
					smallSize[j]->setHealth(smallSize[j]->getHealth() - bulletArray[i]->getDamage());
					bulletArray[i]->setIsDead(true);
					bulletArray[i]->position = glm::vec2(0.0f, 200.0f);
					cout << "bullet hit small asteroid at index " << j << endl;

					//change asteroid acceleration
					force = bulletArray[i]->getMass() * bulletArray[i]->getAcceleration();
					smallSize[j]->setAcceleration(smallSize[j]->getAcceleration() + (force / smallSize[j]->getMass()));

					//change asteroid oriantation
					smallSize[j]->setOrient(smallSize[j]->getOrient() + playerShip->orientation * float(tDelta));

					//check if asteroid is destroyed
					
					if (smallSize[j]->getHealth() <= 0)
					{
						smallSize[j]->setIsDead(true);
						smallSize[j]->position = glm::vec2(200.0f, 0.0f);
						cout << "small asteroid destroyed at index " << j << endl;
					}
					
				}
			}
		}
	}

	for (int a = 0; a < 15; a++)//bulletArray
	{
		for (int b = 0; b < 200; b++)//midSize
		{
			if (bulletArray[a]->getIsDead() == false && midSize[b]->getIsDead() == false)
			{
				float distance = glm::distance(bulletArray[a]->position, midSize[b]->position);
				if (distance < 10.0f)//10 is radius of mid asteroid + radius of bullet
				{
					//hit detected
					midSize[b]->setHealth(midSize[b]->getHealth() - bulletArray[a]->getDamage());
					bulletArray[a]->setIsDead(true);
					bulletArray[a]->position = glm::vec2(0.0f, 200.0f);
					cout << "bullet hit mid asteroid at index " << b << endl;

					//change asteroid acceleration
					force = bulletArray[a]->getMass() * bulletArray[a]->getAcceleration();
					midSize[b]->setAcceleration(midSize[b]->getAcceleration() + (force / midSize[b]->getMass()));

					//change asteroid oriantation
					midSize[b]->setOrient(midSize[b]->getOrient() + playerShip->orientation * float(tDelta));

					//check if asteroid is destroyed
					if (midSize[b]->getHealth() <= 0)
					{
						midSize[b]->setIsDead(true);
						midSize[b]->position = glm::vec2(200.0f, 0.0f);
						cout << "mid asteroid destroyed at index " << b << endl;
					}
				}
			}
		}
		
	}

	for (int z = 0; z < 15; z++)//bulletArray
	{
		for (int y = 0; y < 100; y++)//bigSize
		{
			if (bulletArray[z]->getIsDead() == false && bigSize[y]->getIsDead() == false)
			{
				float distance = glm::distance(bulletArray[z]->position, bigSize[y]->position);
				if (distance < 5.0f)//5 is radius of small asteroid + radius of bullet
				{
					//hit detected
					bigSize[y]->setHealth(bigSize[y]->getHealth() - bulletArray[z]->getDamage());
					bulletArray[z]->setIsDead(true);
					bulletArray[z]->position = glm::vec2(0.0f, 200.0f);
					cout << "bullet hit small asteroid at index " << y << endl;

					//change asteroid acceleration
					force = bulletArray[z]->getMass() * bulletArray[z]->getAcceleration();
					bigSize[y]->setAcceleration(bigSize[y]->getAcceleration() + (force / bigSize[y]->getMass()));

					//change asteroid oriantation
					// 
					// idk how to do this yet
					// 
					//bigSize[y]->setOrient(bigSize[y]->getOrient() + playerShip->orientation * float(tDelta));
					//glm::vec2 bulDir = glm::vec2(cos(bulletArray[z]->orientation), sin(bulletArray[z]->orientation));
					//glm::vec2 astDir = glm::vec2(cos(bigSize[y]->orientation), sin(bigSize[y]->orientation));

					//glm::vec2 normalDir = glm::normalize(bulDir - astDir);


					//check if asteroid is destroyed
					if (bigSize[y]->getHealth() <= 0)
					{
						bigSize[y]->setIsDead(true);
						bigSize[y]->position = glm::vec2(200.0f, 0.0f);
						cout << "small asteroid destroyed at index " << y << endl;
					}
				}
			}
		}
	}
	for (int e = 0; e < 15; e++)//bulletArray
	{
		for (int f = 0; f < 10; f++)//uFOs
		{
			if (bulletArray[e]->getIsDead() == false && alienUFOs[f]->getIsDead() == false)
			{
				float distance = glm::distance(bulletArray[e]->position, alienUFOs[f]->position);
				if (distance < 5.0f)//5 is radius of small asteroid + radius of bullet
				{
					//hit detected
					alienUFOs[f]->setHealth(alienUFOs[f]->getHealth() - bulletArray[e]->getDamage());
					bulletArray[e]->setIsDead(true);
					bulletArray[e]->position = glm::vec2(0.0f, 200.0f);
					cout << "bullet hit small asteroid at index " << f << endl;

					if (alienUFOs[f]->getHealth() <= 0)
					{
						alienUFOs[f]->setIsDead(true);
						alienUFOs[f]->position = glm::vec2(200.0f, 0.0f);
						cout << "small asteroid destroyed at index " << f << endl;
					}
				}
			}
		}
	}
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
	//glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead, float orientationAcceleration
	for (int i = 0; i < 400; i++)
	{
		smallSize[i] = new AstrodsBase(glm::vec2(200.0f, 0.0f), glm::radians((float)(rand() % 360)), glm::vec2(5.0f, 5.0f), smallTextureID, 5, 10, 10.0f, 5.0f, true, 1.0f);
	}
	for (int j = 0; j < 200; j++)
	{
		midSize[j] = new AstrodsBase(glm::vec2(200.0f, 0.0f), glm::radians((float)(rand() % 360)), glm::vec2(10.0f, 10.0f), midTextureID, 10, 20, 20.0f, 2.0f, true, 0.5f);
	}
	for (int z = 0; z < 100; z++)
	{
		bigSize[z] = new AstrodsBase(glm::vec2(200.0f, 0.0f), glm::radians((float)(rand() % 360)), glm::vec2(20.0f, 20.0f), largTextureID, 20, 40, 40.0f, 0.5f, true, 0.1f);
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

	//UFO array
	for (int y = 0; y < 10; y++)//glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass, float initAcceleration, bool initIsDead, float initorientationSpeed, bool initIsShooting
	{
		alienUFOs[y] = new UFO(glm::vec2(200.0f, 200.0f), glm::radians(glm::radians(90.0f)), glm::vec2(5.0f, 5.0f), UFOTextureID, 15, 500, 50.0f, 10.0f, true, 5.0f, false);
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
	for (int i = 0; i < 400; i++)
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
	for (int j = 0; j < 200; j++)
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
			if (midSize[j]->position.y - 5 < height && midSize[j]->position.y + 5 > -height)
			{
				//do nothing
			}
			else
			{
				midSize[j]->position.y *= -1.0f;
			}
		}
	}
	for (int z = 0; z < 100; z++)
	{
		if (bigSize[z]->getIsDead() == false)//20
		{
			if (bigSize[z]->position.x - 7 < width && bigSize[z]->position.x + 7 > -width)
			{
				//do nothing
			}
			else
			{
				bigSize[z]->position.x *= -1.0f;
			}
			if (bigSize[z]->position.y - 7 < height && bigSize[z]->position.y + 7 > -height)
			{
				//do nothing
			}
			else
			{
				bigSize[z]->position.y *= -1.0f;
			}
		}
	}
	for (int e = 0; e < 10; e++)
	{
		if (alienUFOs[e]->getIsDead() == false)//20
		{
			if (alienUFOs[e]->position.x - 5 < width && alienUFOs[e]->position.x + 5 > -width)
			{
				//do nothing
			}
			else
			{
				alienUFOs[e]->position.x *= -1.0f;
			}
			if (alienUFOs[e]->position.y - 5 < height && alienUFOs[e]->position.y + 5 > -height)
			{
				//do nothing
			}
			else
			{
				alienUFOs[e]->position.y *= -1.0f;
			}
		}
	}
}


void orientationAsteroids(double tDelta)
{
	for (int i = 0; i < 400; i++)
	{
		if (smallSize[i]->getIsDead() == false)
		{
			smallSize[i]->orientation += smallSize[i]->getOrientationAcceleration() * (float)tDelta;
		}
	}
	for (int j = 0; j < 200; j++)
	{
		if (midSize[j]->getIsDead() == false)
		{
			midSize[j]->orientation += midSize[j]->getOrientationAcceleration() * (float)tDelta;
		}
	}
	for (int z = 0; z < 100; z++)
	{
		if (bigSize[z]->getIsDead() == false)
		{
			bigSize[z]->orientation += bigSize[z]->getOrientationAcceleration() * (float)tDelta;
		}
	}
}

void setUpLevel()
{
	astrodsThatIsDead = 0;
	
	for (int i = 0; i < 400; i++)
	{
		if (smallSize[i]->getIsDead() == true) 
		{
			astrodsThatIsDead += 1;
		}
	}
	for (int j = 0; j < 200; j++)
	{
		if (midSize[j]->getIsDead() == true)
		{
			astrodsThatIsDead += 1;
		}
	}
	for (int z = 0; z < 100; z++)
	{
		if (bigSize[z]->getIsDead() == true)
		{
			astrodsThatIsDead += 1;
		}
	}

	if (astrodsThatIsDead >= 700)
	{
		level += 1;
		cout << "Level up! Current level: " << level << endl;
		spawnAsteroid();
	}

}

void spawnAsteroid()
{
	int type;
	float temp1;
	float temp2;
	difficultyPoints = 100;
	difficultyPoints *= level;
	while (difficultyPoints > 0)
	{
		temp1 = 0.0f;
		temp2 = 0.0f;
		printf("difficultyPoints is %d\n", difficultyPoints);

		if (level < 5)
		{
			type = rand() % 3;
			if (type == 0)
			{
				//spawn big asteroid
				for (int z = 0; z < 100; z++)
				{
					if (bigSize[z]->getIsDead())
					{
						bigSize[z]->setIsDead(false);
						difficultyPoints -= bigSize[z]->getHealth();

						while (temp1 >= -40 && temp1 <= 40 && temp2 >= -40 && temp2 <= 40)
						{
							temp1 = (float)(rand() % 100 - 50);
							temp2 = (float)(rand() % 100 - 50);
						}

						bigSize[z]->position = glm::vec2(temp1, temp2);
						bigSize[z]->orientation = glm::radians((float)(rand() % 360));
						bigSize[z]->setHealth(40);
						break;
					}
				}

			}
			else if (type == 1)
			{
				//spawn mid asteroid
				for (int j = 0; j < 200; j++)
				{
					if (midSize[j]->getIsDead())
					{
						midSize[j]->setIsDead(false);
						difficultyPoints -= midSize[j]->getHealth();
						while (temp1 >= -45 && temp1 <= 45 && temp2 >= -45 && temp2 <= 45)
						{
							temp1 = (float)(rand() % 100 - 50);
							temp2 = (float)(rand() % 100 - 50);
						}

						midSize[j]->position = glm::vec2(temp1, temp2);
						midSize[j]->orientation = glm::radians((float)(rand() % 360));
						midSize[j]->setHealth(20);
						break;
					}
				}
			}
			else
			{
				//spawn small asteroid
				for (int i = 0; i < 400; i++)
				{
					if (smallSize[i]->getIsDead())
					{
						smallSize[i]->setIsDead(false);
						difficultyPoints -= smallSize[i]->getHealth();
						while (temp1 >= -45 && temp1 <= 45 && temp2 >= -45 && temp2 <= 45)
						{
							temp1 = (float)(rand() % 100 - 50);
							temp2 = (float)(rand() % 100 - 50);
						}

						smallSize[i]->position = glm::vec2(temp1, temp2);
						smallSize[i]->orientation = glm::radians((float)(rand() % 360));
						smallSize[i]->setHealth(10);
						break;
					}
				}
			}
		}
		else
		{
			type = rand() % 4;
			if (type == 0)
			{
				//spawn big asteroid
				for (int z = 0; z < 100; z++)
				{
					if (bigSize[z]->getIsDead())
					{
						bigSize[z]->setIsDead(false);
						difficultyPoints -= bigSize[z]->getHealth();
						while (temp1 >= -50 && temp1 <= 50 && temp2 >= -50 && temp2 <= 50)
						{
							temp1 = (float)(rand() % 120 - 60);
							temp2 = (float)(rand() % 120 - 60);
						}

						bigSize[z]->position = glm::vec2(temp1, temp2);
						bigSize[z]->orientation = glm::radians((float)(rand() % 360));
						bigSize[z]->setHealth(40);
						break;
					}
				}

			}
			else if (type == 1)
			{
				//spawn mid asteroid
				for (int j = 0; j < 200; j++)
				{
					if (midSize[j]->getIsDead())
					{
						midSize[j]->setIsDead(false);
						difficultyPoints -= midSize[j]->getHealth();
						while (temp1 >= -50 && temp1 <= 50 && temp2 >= -50 && temp2 <= 50)
						{
							temp1 = (float)(rand() % 120 - 60);
							temp2 = (float)(rand() % 120 - 60);
						}

						midSize[j]->position = glm::vec2(temp1, temp2);
						midSize[j]->orientation = glm::radians((float)(rand() % 360));
						midSize[j]->setHealth(20);
						break;
					}
				}
			}
			else if (type == 2)
			{
				//spawn small asteroid
				for (int i = 0; i < 400; i++)
				{
					if (smallSize[i]->getIsDead())
					{
						smallSize[i]->setIsDead(false);
						difficultyPoints -= smallSize[i]->getHealth();
						while (temp1 >= -50 && temp1 <= 50 && temp2 >= -50 && temp2 <= 50)
						{
							temp1 = (float)(rand() % 120 - 60);
							temp2 = (float)(rand() % 120 - 60);
						}

						smallSize[i]->position = glm::vec2(temp1, temp2);
						smallSize[i]->orientation = glm::radians((float)(rand() % 360));
						smallSize[i]->setHealth(10);
						break;
					}
				}
			}
			else
			{
				//spawn alien UFO
				for (int y = 0; y < 10; y++)
				{
					if (alienUFOs[y]->getIsDead())
					{
						alienUFOs[y]->setIsDead(false);
						difficultyPoints -= alienUFOs[y]->getHealth();
						alienUFOs[y]->position = glm::vec2((float)(rand() % 100 - 50), (float)(rand() % 100 - 50));
						alienUFOs[y]->orientation = glm::radians((float)(rand() % 360));
						break;
					}
				}
			}
		}

		
	}
}

void movementAsteroids(double tDelta)
{
	for (int i = 0; i < 400; i++)
	{
		if (smallSize[i]->getIsDead() == false)
		{
			dx = smallSize[i]->getAcceleration() * cos(smallSize[i]->getOrient()) * (float)tDelta;
			dy = smallSize[i]->getAcceleration() * sin(smallSize[i]->getOrient()) * (float)tDelta;

			smallSize[i]->position.x += dx;
			smallSize[i]->position.y += dy;
		}
	}
	for (int j = 0; j < 200; j++)
	{
		if (midSize[j]->getIsDead() == false)
		{
			dx = midSize[j]->getAcceleration() * cos(midSize[j]->getOrient()) * (float)tDelta;
			dy = midSize[j]->getAcceleration() * sin(midSize[j]->getOrient()) * (float)tDelta;

			midSize[j]->position.x += dx;
			midSize[j]->position.y += dy;
		}
	}
	for (int z = 0; z < 100; z++)
	{
		if (bigSize[z]->getIsDead() == false)
		{
			dx = bigSize[z]->getAcceleration() * cos(bigSize[z]->getOrient()) * (float)tDelta;
			dy = bigSize[z]->getAcceleration() * sin(bigSize[z]->getOrient()) * (float)tDelta;

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
	for (int i = 0; i < 400; i++)
	{
		if (smallSize[i]->getIsDead())
		{
			continue;
		}
		smallSize[i]->render();
	}
	for (int j = 0; j < 200; j++)
	{
		if (midSize[j]->getIsDead())
		{
			continue;
		}
		midSize[j]->render();
	}
	for (int z = 0; z < 100; z++)
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

	// Render UFOs
	for (int y = 0; y < 10; y++)
	{
		if (alienUFOs[y]->getIsDead())
		{
			continue;
		}
		alienUFOs[y]->render();
	}
}



