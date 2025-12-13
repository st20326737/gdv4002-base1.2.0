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
const float maxSpeed = 100.0f;
const float maxTurn = 150.0f;
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
float movetimer = 0.0f;
float moveCooldown = 0.01f;
float asterHitPlayerCooldown = 10.0f;
float asterHitPlayerTimer = 0.0f;


//arrays
AstrodsBase* smallSize[400];
AstrodsBase* midSize[200];
AstrodsBase* bigSize[100];

ProjectilesBase* bulletArray[15];
ProjectilesBase* enemyBulletArray[50];
ProjectilesBase* missileArray[2];

UFO* alienUFOs[10];




// Function prototypes
void impactVelocity(ProjectilesBase* bullet, AstrodsBase* asteroid);
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
void shoot(float tDelta);
void turnShip(float tDelta);
void accelerateShip(float tDelta);
void spawningAsteroid(float temp1, float temp2, AstrodsBase* astrod);

void splitAsteroidBig(AstrodsBase* asteroid);
void splitAsteroidMid(AstrodsBase* asteroid);

void asteroidTouchPlayer(GLFWwindow* window, PlayerShip* player, AstrodsBase* asteroid, float tDelta);
void touch(GLFWwindow* window, float tDelta);
void asteroidContactAsteroid(AstrodsBase* asteroid1, AstrodsBase* asteroid2);

void checkIfAsteroidContactAsteroidSMALL();
void checkIfAsteroidContactAsteroidMID();
void checkIfAsteroidContactAsteroidBIG();

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_ALWAYS);


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
	shoot((float)tDelta);

	flyBullet(tDelta);

	bulletOffScreen(getViewplaneWidth() / 2.0f, getViewplaneHeight() / 2.0f);

	//turn
	turnShip((float)tDelta);


	//accelerate
	accelerateShip((float)tDelta);

	//check for asteroid touching player
	touch(window, (float)tDelta);

	//saddly this lags the game out to the point of unplayable, too many loops
	/*
	//check for asteroid touching asteroid
	checkIfAsteroidContactAsteroidSMALL();
	checkIfAsteroidContactAsteroidMID();
	checkIfAsteroidContactAsteroidBIG();
	*/
	

}
void impactVelocity(ProjectilesBase* bullet, AstrodsBase* asteroid)
{
	float force1, force2, force3, temp, a, v;

	force1 = bullet->getMass() * bullet->getAcceleration();
	force2 = asteroid->getMass() * asteroid->getAcceleration();

	//change asteroid acceleration
	glm::vec2 dirt1 = bullet->getDir();
	glm::vec2 dirt2 = asteroid->getDir();
	temp = glm::dot(glm::normalize(dirt1), glm::normalize(dirt2));
	if (temp < 0)
	{
		if ((asteroid->getOrient() + glm::radians(180.0f)) > bullet->orientation && asteroid->getVelocity() <= 0)
		{
			asteroid->setOrient(asteroid->getOrient() + (bullet->orientation - asteroid->getOrient()) * 0.5);
			asteroid->setVelocity(fabs(asteroid->getVelocity()));
		}
		if ((asteroid->getOrient() - glm::radians(180.0f)) < bullet->orientation && asteroid->getVelocity() <= 0)
		{
			asteroid->setOrient(asteroid->getOrient() - (asteroid->getOrient() - bullet->orientation) * 0.5);
			asteroid->setVelocity(fabs(asteroid->getVelocity()));
		}
		force3 = -(force2 - force1);

	}
	else
	{
		if ((asteroid->getOrient() + glm::radians(180.0f)) > bullet->orientation)
		{
			asteroid->setOrient(asteroid->getOrient() + (bullet->orientation - asteroid->getOrient()) * 0.5);
		}
		if ((asteroid->getOrient() - glm::radians(180.0f)) < bullet->orientation)
		{
			asteroid->setOrient(asteroid->getOrient() - (asteroid->getOrient() - bullet->orientation) * 0.5);
		}
		asteroid->setOrient(asteroid->getOrient() + (bullet->orientation - asteroid->getOrient()) * 0.5);
		force3 = force2 + force1;
	}

	a = force3 / asteroid->getMass();
	v = asteroid->getVelocity() + a;
	asteroid->setVelocity(v);

}

void checkIfAsteroidContactAsteroidSMALL()
{
	for (int i = 0; i < 400; i++)//small to small
	{
		for (int j = 0; j < 400; j++)
		{
			if (i != j)
			{
				asteroidContactAsteroid(smallSize[i], smallSize[j]);
			}
			
		}
	}
	for (int z = 0; z < 400; z++)//small to mid
	{
		for (int y = 0; y < 200; y++)
		{
			asteroidContactAsteroid(smallSize[z], midSize[y]);
		}
	}
	for (int a = 0; a < 400; a++)//small to big
	{
		for (int b = 0; b < 100; b++)
		{
			asteroidContactAsteroid(smallSize[a], bigSize[b]);
		}
	}
}

void checkIfAsteroidContactAsteroidMID()
{
	for (int i = 0; i < 200; i++)//mid to small
	{
		for (int j = 0; j < 400; j++)
		{
			asteroidContactAsteroid(midSize[i], smallSize[j]);
		}
	}
	for (int z = 0; z < 200; z++)//mid to mid
	{
		for (int y = 0; y < 200; y++)
		{
			if (z != y)
			{
				asteroidContactAsteroid(midSize[z], midSize[y]);
			}
			
		}
	}
	for (int a = 0; a < 200; a++)//mid to big
	{
		for (int b = 0; b < 100; b++)
		{
			asteroidContactAsteroid(midSize[a], bigSize[b]);
		}
	}
}

void checkIfAsteroidContactAsteroidBIG()//this needs changes of numbers
{
	for (int i = 0; i < 100; i++)//big to small
	{
		for (int j = 0; j < 400; j++)
		{
			asteroidContactAsteroid(bigSize[i], smallSize[j]);
		}
	}
	for (int z = 0; z < 100; z++)//big to mid
	{
		for (int y = 0; y < 200; y++)
		{
			asteroidContactAsteroid(bigSize[z], midSize[y]);
		}
	}
	for (int a = 0; a < 100; a++)//big to big
	{
		for (int b = 0; b < 100; b++)
		{
			if (a != b)
			{
				asteroidContactAsteroid(bigSize[a], bigSize[b]);
			}
			
		}
	}
}

void asteroidContactAsteroid(AstrodsBase* asteroid1, AstrodsBase* asteroid2)//concept for future use
{
	float force1, force2, force3, temp, a1, a2, v1, v2;
	//small 5, mid 10, big 20
	// 
	//handle hit
	float distance = glm::distance(asteroid1->position, asteroid2->position);
	if (distance < 10.0f)//10 is radius of asteroid for testing
	{
		force1 = asteroid1->getMass() * asteroid1->getAcceleration();
		force2 = asteroid2->getMass() * asteroid2->getAcceleration();

		glm::vec2 dirt1 = asteroid1->getDir();
		glm::vec2 dirt2 = asteroid2->getDir();

		temp = glm::dot(glm::normalize(dirt1), glm::normalize(dirt2));

		if (temp < 0)
		{
			if ((asteroid1->getOrient() + glm::radians(180.0f)) > asteroid2->getOrient() && asteroid1->getVelocity() <= 0)
			{
				asteroid1->setOrient(asteroid1->getOrient() + (asteroid2->getOrient() - asteroid1->getOrient()) * 0.5);
				asteroid1->setVelocity(fabs(asteroid1->getVelocity()));
			}
			if ((asteroid1->getOrient() - glm::radians(180.0f)) < asteroid2->getOrient() && asteroid1->getVelocity() <= 0)
			{
				asteroid1->setOrient(asteroid1->getOrient() - (asteroid1->getOrient() - asteroid2->getOrient()) * 0.5);
				asteroid1->setVelocity(fabs(asteroid1->getVelocity()));
			}

			if ((asteroid2->getOrient() + glm::radians(180.0f)) > asteroid1->getOrient() && asteroid2->getVelocity() <= 0)
			{
				asteroid2->setOrient(asteroid2->getOrient() + (asteroid1->getOrient() - asteroid2->getOrient()) * 0.5);
				asteroid2->setVelocity(fabs(asteroid2->getVelocity()));
			}
			if ((asteroid2->getOrient() - glm::radians(180.0f)) < asteroid1->getOrient() && asteroid2->getVelocity() <= 0)
			{
				asteroid2->setOrient(asteroid2->getOrient() - (asteroid2->getOrient() - asteroid1->getOrient()) * 0.5);
				asteroid2->setVelocity(fabs(asteroid2->getVelocity()));
			}

			force3 = -(force2 - force1);
		}
		else
		{
			if ((asteroid1->getOrient() + glm::radians(180.0f)) > asteroid2->getOrient())
			{
				asteroid1->setOrient(asteroid1->getOrient() + (asteroid2->getOrient() - asteroid1->getOrient()) * 0.5);
			}
			if ((asteroid1->getOrient() - glm::radians(180.0f)) < asteroid2->getOrient())
			{
				asteroid1->setOrient(asteroid1->getOrient() - (asteroid1->getOrient() - asteroid2->getOrient()) * 0.5);
			}

			if ((asteroid2->getOrient() + glm::radians(180.0f)) > asteroid1->getOrient())
			{
				asteroid2->setOrient(asteroid2->getOrient() + (asteroid1->getOrient() - asteroid2->getOrient()) * 0.5);
			}
			if ((asteroid2->getOrient() - glm::radians(180.0f)) < asteroid1->getOrient())
			{
				asteroid2->setOrient(asteroid2->getOrient() - (asteroid2->getOrient() - asteroid1->getOrient()) * 0.5);
			}

			asteroid1->setOrient(asteroid1->getOrient() + (asteroid2->getOrient() - asteroid1->getOrient()) * 0.5);
			asteroid2->setOrient(asteroid2->getOrient() + (asteroid1->getOrient() - asteroid2->getOrient()) * 0.5);
			force3 = force2 + force1;
		}

		a1 = force3 / asteroid1->getMass();
		v1 = asteroid1->getVelocity() + a1;
		asteroid1->setVelocity(v1);

		a2 = force3 / asteroid2->getMass();
		v2 = asteroid2->getVelocity() + a2;
		asteroid2->setVelocity(v2);

	}
}


void asteroidTouchPlayer(GLFWwindow* window, PlayerShip* player, AstrodsBase* asteroid, float tDelta)
{
	//handle hit
	asterHitPlayerTimer += (float)tDelta;

	if (asterHitPlayerTimer > asterHitPlayerCooldown)
	{
		float distance = glm::distance(player->position, asteroid->position);
		if (distance < 10.0f)//10 is radius of asteroid + radius of player ship
		{
			//hit detected
			asterHitPlayerTimer = 0.0f;
			player->setHealth(player->getHealth() - asteroid->getDamage());
			asteroid->setHealth(asteroid->getHealth() - player->getDamage());
			cout << "  " << endl;
			cout << "player hit by asteroid" << endl;
			cout << "player health: " << player->getHealth() << endl;
			//check if player is destroyed
			if (player->getHealth() <= 0)
			{
				player->setIsDead(true);
				cout << "player destroyed" << endl;
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
		}
		
	}
	
}

void touch(GLFWwindow* window, float tDelta)
{
	for (int i = 0; i < 400; i++)
	{
		if (!(smallSize[i]->getIsDead()))
		{
			asteroidTouchPlayer(window, playerShip, smallSize[i], tDelta);
		}
	}
	for (int j = 0; j < 200; j++)
	{
		if (!(midSize[j]->getIsDead()))
		{
			asteroidTouchPlayer(window, playerShip, midSize[j], tDelta);
		}
	}
	for (int k = 0; k < 100; k++)
	{
		if (!(bigSize[k]->getIsDead()))
		{
			asteroidTouchPlayer(window, playerShip, bigSize[k], tDelta);
		}
	}
}

void accelerateShip(float tDelta)
{
	movetimer += (float)tDelta;

	if (movetimer >= moveCooldown) 
	{
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

			if (turnVelocity > 0)
			{
				turnVelocity -= turnAcceleration;
			}
			else if (turnVelocity < 0)
			{
				turnVelocity += forwardAcceleration;
			}
		}

		if (fabs(forwardVelocity) > maxSpeed)
		{
			forwardVelocity = (forwardVelocity / fabs(forwardVelocity)) * maxSpeed;//scale to max speed with sign
		}
		movetimer = 0.0f;
	}
	dx = forwardVelocity * cos(playerShip->orientation) * (float)tDelta;
	dy = forwardVelocity * sin(playerShip->orientation) * (float)tDelta;

	playerShip->position.x += dx;
	playerShip->position.y += dy;
}

void turnShip(float tDelta)
{
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
}

void shoot(float tDelta)
{
	shoottimer += (float)tDelta;

	if (shoottimer >= shootCooldown)
	{
		if (spacePressed)
		{
			shootBullet(playerShip, (float)tDelta);
			shoottimer = 0.0f;
		}
	}
}

void bulletHit(double tDelta)
{
	//mass * acceleration = force
	//therefore the transfur of force to acceleration is force / mass = acceleration

	//oriantation = oriantation + bullet oriantation * tDelta

	float force1, force2, temp;
	glm::vec2 dirt1, dirt2;

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
					cout << "  " << endl;
					cout << "bullet hit small asteroid at index " << j << endl;

					// Change impact velocity
					impactVelocity(bulletArray[i], smallSize[j]);

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
					cout << "  " << endl;
					cout << "bullet hit mid asteroid at index " << b << endl;

					impactVelocity(bulletArray[a], midSize[b]);

					//check if asteroid is destroyed
					if (midSize[b]->getHealth() <= 0)
					{
						midSize[b]->setIsDead(true);
						splitAsteroidMid(midSize[b]);
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
					cout << "  " << endl;
					cout << "bullet hit big asteroid at index " << y << endl;

					impactVelocity(bulletArray[z], bigSize[y]);


					//check if asteroid is destroyed
					if (bigSize[y]->getHealth() <= 0)
					{
						bigSize[y]->setIsDead(true);
						splitAsteroidBig(bigSize[y]);
						bigSize[y]->position = glm::vec2(200.0f, 0.0f);
						cout << "big asteroid destroyed at index " << y << endl;
						
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
					cout << "  " << endl;
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

void splitAsteroidBig(AstrodsBase* asteroid)
{
	float temp1, temp2;
	temp1 = 0.0f;
	temp2 = 0.0f;
	int twoTimes = 0;

	while (twoTimes < 2)
	{
		for (int z = 0; z < 100; z++)
		{
			if (midSize[z]->getIsDead())
			{
				midSize[z]->setIsDead(false);

				//spawn outside of screen
				temp1 = asteroid->position.x;
				temp2 = asteroid->position.y;

				//reset asteroid
				spawningAsteroid(temp1, temp2, midSize[z]);
				midSize[z]->setHealth(20);
				twoTimes++;
				break;
			}
		}
	}
}

void splitAsteroidMid(AstrodsBase* asteroid)
{
	float temp1, temp2;
	temp1 = 0.0f;
	temp2 = 0.0f;
	int twoTimes = 0;

	while (twoTimes < 2)
	{
		for (int z = 0; z < 100; z++)
		{
			if (smallSize[z]->getIsDead())
			{
				smallSize[z]->setIsDead(false);

				temp1 = asteroid->position.x;
				temp2 = asteroid->position.y;

				//reset asteroid
				spawningAsteroid(temp1, temp2, smallSize[z]);
				smallSize[z]->setHealth(10);
				twoTimes++;
				break;
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

		case GLFW_KEY_ESCAPE:
			// If escape is pressed tell GLFW we want to close the window (and quit)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
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
	//glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, int initHealth, float initMass(kg), float initAcceleration, bool initIsDead, float orientationAcceleration
	for (int i = 0; i < 400; i++)
	{
		smallSize[i] = new AstrodsBase(glm::vec2(200.0f, 0.0f), glm::radians((float)(rand() % 360)), glm::vec2(5.0f, 5.0f), smallTextureID, 5, 10, 1000.0f, 5.0f, true, 1.0f, 0.0f, glm::vec2(0.0f,0.0f));
	}
	for (int j = 0; j < 200; j++)
	{
		midSize[j] = new AstrodsBase(glm::vec2(200.0f, 0.0f), glm::radians((float)(rand() % 360)), glm::vec2(10.0f, 10.0f), midTextureID, 10, 20, 2000.0f, 2.0f, true, 0.5f, 0.0f, glm::vec2(0.0f, 0.0f));
	}
	for (int z = 0; z < 100; z++)
	{
		bigSize[z] = new AstrodsBase(glm::vec2(200.0f, 0.0f), glm::radians((float)(rand() % 360)), glm::vec2(20.0f, 20.0f), largTextureID, 20, 40, 4000.0f, 0.5f, true, 0.1f, 0.0f, glm::vec2(0.0f, 0.0f));
	}

	//projectile arrays
	//glm::vec2 initPosition, float initOrientation, glm::vec2 initSize, GLuint initTextureID, int initDamage, bool initIsDead, float initMass(kg), float initAcceleration
	for (int a = 0; a < 15; a++)
	{
		bulletArray[a] = new ProjectilesBase(glm::vec2(0.0f, 200.0f), glm::radians(90.0f), glm::vec2(2.5f, 2.5f), bulletTextureID, 10, true, 1.0f, 250.0f, glm::vec2(0.0f, 0.0f));
		cout << "bullet array set up at index " << a << endl;
		cout << a << ": " << bulletArray[a]->getIsDead() << endl;
	}
	for (int b = 0; b < 50; b++)
	{
		enemyBulletArray[b] = new ProjectilesBase(glm::vec2(0.0f, 200.0f), glm::radians(90.0f), glm::vec2(2.5f, 2.5f), bulletTextureID, 10, true, 1.0f, 250.0f, glm::vec2(0.0f, 0.0f));
	}
	for (int c = 0; c < 2; c++)
	{
		missileArray[c] = new ProjectilesBase(glm::vec2(0.0f, 200.0f), glm::radians(90.0f), glm::vec2(2.5f, 2.5f), bulletTextureID, 50, true, 10.0f, 50.0f, glm::vec2(0.0f, 0.0f));
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

void spawningAsteroid(float temp1, float temp2, AstrodsBase* astrod)
{
	astrod->position = glm::vec2(temp1, temp2);
	astrod->orientation = glm::radians((float)(rand() % 360));
	astrod->setOrient(glm::radians((float)(rand() % 360)));
	astrod->setAcceleration(5.0f);
	astrod->setVelocity(astrod->getAcceleration());
	astrod->setDir(glm::vec2(0.0f, 0.0f));
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
				//glm::vec2(200.0f, 0.0f), glm::radians((float)(rand() % 360)), glm::vec2(5.0f, 5.0f), smallTextureID, 5, 10, 1000.0f, 5.0f, true, 1.0f
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
						//spawningAsteroid(float temp1, float temp2, AstrodsBase* astrod)
						spawningAsteroid(temp1, temp2, bigSize[z]);
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

						spawningAsteroid(temp1, temp2, midSize[j]);
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

						spawningAsteroid(temp1, temp2, smallSize[i]);
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

						spawningAsteroid(temp1, temp2, bigSize[z]);
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
						spawningAsteroid(temp1, temp2, midSize[j]);
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

						spawningAsteroid(temp1, temp2, smallSize[i]);
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
{//bulletArray[i]->setDir(glm::vec2(bulletArray[i]->position.x, bulletArray[i]->position.y));
	for (int i = 0; i < 400; i++)
	{
		if (smallSize[i]->getIsDead() == false)
		{
			dx = smallSize[i]->getVelocity() * cos(smallSize[i]->getOrient()) * (float)tDelta;
			dy = smallSize[i]->getVelocity() * sin(smallSize[i]->getOrient()) * (float)tDelta;

		//	smallSize[i]->setDir(glm::vec2(smallSize[i]->position.x, smallSize[i]->position.y));
			smallSize[i]->setDir(glm::vec2(dx, dy));

			smallSize[i]->position.x += dx;
			smallSize[i]->position.y += dy;
		}
	}
	for (int j = 0; j < 200; j++)
	{
		if (midSize[j]->getIsDead() == false)
		{
			dx = midSize[j]->getVelocity() * cos(midSize[j]->getOrient()) * (float)tDelta;
			dy = midSize[j]->getVelocity() * sin(midSize[j]->getOrient()) * (float)tDelta;

		//	midSize[j]->setDir(glm::vec2(midSize[j]->position.x, midSize[j]->position.y));
			midSize[j]->setDir(glm::vec2(dx, dy));

			midSize[j]->position.x += dx;
			midSize[j]->position.y += dy;
		}
	}
	for (int z = 0; z < 100; z++)
	{
		if (bigSize[z]->getIsDead() == false)
		{
			dx = bigSize[z]->getVelocity() * cos(bigSize[z]->getOrient()) * (float)tDelta;
			dy = bigSize[z]->getVelocity() * sin(bigSize[z]->getOrient()) * (float)tDelta;

		//	bigSize[z]->setDir(glm::vec2(bigSize[z]->position.x, bigSize[z]->position.y));
			bigSize[z]->setDir(glm::vec2(dx, dy));

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

			bulletArray[i]->setDir(glm::vec2(dx, dy));

			bulletArray[i]->position.x += dx;
			bulletArray[i]->position.y += dy;
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



