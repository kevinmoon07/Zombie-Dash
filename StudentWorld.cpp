#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "Level.h"
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
};

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath), m_penelope(nullptr)
{
};

StudentWorld::~StudentWorld()
{
	cleanUp();
};

int StudentWorld::init()
{
	//Initialize the level
	Level lev(assetPath());
	string levelFile;
	int currentLevel = getLevel();
	ostringstream level;
	level << setfill('0') << setw(2) << currentLevel;
	ostringstream leveltot;
	leveltot << "level" << level.str() << ".txt";
	levelFile = leveltot.str();

	//Read out error messages
	Level::LoadResult result = lev.loadLevel(levelFile);

	if (result == Level::load_fail_file_not_found || currentLevel == 100)
	{
		return GWSTATUS_PLAYER_WON;
	}
	else if (result == Level::load_fail_bad_format)
	{
		return GWSTATUS_LEVEL_ERROR;
	}
	else if (result == Level::load_success)
	{

		//Initialize the objects in the level
		for (int x = 0; x < LEVEL_WIDTH; ++x)
		{
			for (int y = 0; y < LEVEL_HEIGHT; ++y)
			{
				Level::MazeEntry ge = lev.getContentsOf(x, y);
				switch (ge)
				{
				case Level::player:
					m_penelope = new Penelope(x, y, this);
					break;
				case Level::wall:
					objects.push_back(new Wall(x, y, this));
					break;
				case Level::empty:
					break;
				case Level::dumb_zombie:
					objects.push_back(new DumbZombie(x, y, this));
					break;
				case Level::smart_zombie:
					objects.push_back(new SmartZombie(x, y, this));
					break;
				case Level::citizen:
					objects.push_back(new Citizen(x, y, this));
					break;
				case Level::exit:
					objects.push_back(new Exit(x, y, this));
					break;
				case Level::pit:
					objects.push_back(new Pit(x, y, this));
					break;
				case Level::vaccine_goodie:
					objects.push_back(new VaccineGoodie(x, y, this));
					break;
				case Level::gas_can_goodie:
					objects.push_back(new GasCanGoodie(x, y, this));
					break;
				case Level::landmine_goodie:
					objects.push_back(new LandmineGoodie(x, y, this));
					break;
				default:
					break;

				}
			}
		}

	}
	return GWSTATUS_CONTINUE_GAME;
};

int StudentWorld::move()
{
	//Output the score
	ostringstream score;
	if (getScore() < 0)
	{
		score << "Score: -" << setfill('0') << setw(5) << abs(getScore());
	}
	else
	{
		score << "Score: ";
		score << setfill('0') << setw(6) << getScore();
	}
	ostringstream level;
	level << "Level: " << getLevel();
	ostringstream lives;
	lives << "Lives: " << getLives();
	ostringstream vaccines;
	vaccines << "Vaccines: " << m_penelope->getVaccines();
	ostringstream flames;
	flames << "Flame: " << m_penelope->getFlames();
	ostringstream mines;
	mines << "Mines: " << m_penelope->getMines();
	ostringstream infected;
	infected << "Infected: " << m_penelope->getInfected();

	ostringstream total;
	total << score.str() << "  " << level.str() << "  " << lives.str() << "  " << vaccines.str() << "  " << flames.str() << "  " << mines.str() << "  " << infected.str();
	setGameStatText(total.str());


	//Have the player do something
	m_penelope->doSomething();

	//Determine if the game is over due to player death
	if (!m_penelope->isAlive())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	//Have each actor do something and determine if the level is complete
	list<Actor*>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it)->canDie() && (*it)->isAlive())
		{
			(*it)->doSomething();
		}
		else if (!(*it)->canDie())
		{
			(*it)->doSomething();
		}

		if (levelComplete())
		{
			playSound(SOUND_LEVEL_FINISHED);
			return GWSTATUS_FINISHED_LEVEL;
		}
	}

	//Determine if the game is over due to player death
	if (!m_penelope->isAlive())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	//Delete actors that have died
	it = objects.begin();
	while (it != objects.end())
	{
		if ((*it)->canDie()) //If an object can die and if it has died, delete it
		{
			list<Actor*>::iterator itTemp;
			if (!(*it)->isAlive())
			{
				itTemp = it;
				delete *itTemp;
				it = objects.erase(itTemp);
			}
			else
			{
				++it;
			}
		}
		else
		{
			++it;
		}
	}

	//Update game status line

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	//Delete the list and the player
	list<Actor*>::iterator it;
	while (!objects.empty())
	{
		it = objects.end();
		--it;
		delete *it;
		objects.erase(it);
	}

	delete m_penelope;
};

bool StudentWorld::levelComplete()
{
	//Determine if all the allowed actors have exited
	//Go through each object and see if all allowable objects have exited	
	list<Actor*>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it)->isAlive() && (*it)->canExit() && (*it)->hasExited())
		{
		}
		else if ((*it)->isAlive() && (*it)->canExit() && !(*it)->hasExited())
		{
			return false;
		}
	}

	if (!m_penelope->hasExited())
	{
		return false;
	}
	return true;
}

bool StudentWorld::isntBlocking(double firstX, double firstY, double blockableX, double blockableY, int pixelCheck, int dir)
{
	//Determine all (x,y) coordinates for the two objects being compared
	double x = firstX;
	double y = firstY;

	double livingBotLeftX = x;
	double livingBotLeftY = y;
	double livingBotRightX = x + SPRITE_WIDTH - 1;
	double livingBotRightY = y;
	double livingTopLeftX = x;
	double livingTopLeftY = y + SPRITE_HEIGHT - 1;
	double livingTopRightX = x + SPRITE_WIDTH - 1;
	double livingTopRightY = y + SPRITE_HEIGHT - 1;

	double blockableBotLeftX = blockableX;
	double blockableBotLeftY = blockableY;
	double blockableBotRightX = blockableX + SPRITE_WIDTH - 1;
	double blockableBotRightY = blockableY;
	double blockableTopLeftX = blockableX;
	double blockableTopLeftY = blockableY + SPRITE_HEIGHT - 1;
	double blockableTopRightX = blockableX + SPRITE_WIDTH - 1;
	double blockableTopRightY = blockableY + SPRITE_HEIGHT - 1;

	//Check to see if the movement for the living charcter crosses any boundaries

	switch (dir)
	{
	case 0:
		if ((((blockableBotLeftX <= livingTopRightX + pixelCheck) && (livingTopRightX + pixelCheck <= blockableBotRightX)) && ((blockableBotLeftY <= livingTopRightY) && (livingTopRightY <= blockableTopLeftY)))
			|| (((blockableBotLeftX <= livingBotRightX + pixelCheck) && (livingBotRightX + pixelCheck <= blockableBotRightX)) && ((blockableBotLeftY <= livingBotRightY) && (livingBotRightY <= blockableTopLeftY))))
		{
			return false;
		}
		break;
	case 1:
		if ((((blockableBotLeftX <= livingTopLeftX) && (livingTopLeftX <= blockableBotRightX)) && ((blockableBotLeftY <= livingTopLeftY + pixelCheck) && (livingTopLeftY + pixelCheck <= blockableTopLeftY)))
			|| (((blockableBotLeftX <= livingTopRightX) && (livingTopRightX <= blockableBotRightX)) && ((blockableBotLeftY <= livingTopRightY + pixelCheck) && (livingTopRightY + pixelCheck <= blockableTopRightY))))
		{
			return false;
		}
		break;
	case 2:
		if ((((blockableBotLeftX <= livingTopLeftX - pixelCheck) && (livingTopLeftX - pixelCheck <= blockableBotRightX)) && ((blockableBotRightY <= livingTopLeftY) && (livingTopLeftY <= blockableTopRightY)))
			|| (((blockableBotLeftX <= livingBotLeftX - pixelCheck) && (livingBotLeftX - pixelCheck <= blockableBotRightX)) && ((blockableBotRightY <= livingBotLeftY) && (livingBotLeftY <= blockableTopRightY))))
		{
			return false;
		}
		break;
	case 3:
		if ((((blockableTopLeftX <= livingBotLeftX) && (livingBotLeftX <= blockableTopRightX)) && ((blockableBotLeftY <= livingBotLeftY - pixelCheck) && (livingBotLeftY - pixelCheck <= blockableTopLeftY)))
			|| (((blockableTopLeftX <= livingBotRightX) && (livingBotRightX <= blockableTopRightX)) && ((blockableBotLeftY <= livingBotRightY - pixelCheck) && (livingBotRightY - pixelCheck <= blockableTopLeftY))))
		{
			return false;
		}
		break;
	default:
		return true;
	}
	return true;
}


bool StudentWorld::wallIsBlocking(double firstX, double firstY)
{
	//Determine all (x,y) coordinates for the two objects being compared
	double x = firstX;
	double y = firstY;

	double livingBotLeftX = x;
	double livingBotLeftY = y;
	double livingBotRightX = x + SPRITE_WIDTH - 1;
	double livingBotRightY = y;
	double livingTopLeftX = x;
	double livingTopLeftY = y + SPRITE_HEIGHT - 1;
	double livingTopRightX = x + SPRITE_WIDTH - 1;
	double livingTopRightY = y + SPRITE_HEIGHT - 1;

	double blockableBotLeftX = 0;
	double blockableBotLeftY = 0;
	double blockableBotRightX = 0;
	double blockableTopLeftY = 0;

	list<Actor*>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it)->canOverlapFlamethrower())
		{
			blockableBotLeftX = (*it)->getX();
			blockableBotLeftY = (*it)->getY();
			blockableBotRightX = (*it)->getX() + SPRITE_WIDTH - 1;
			blockableTopLeftY = (*it)->getY() + SPRITE_HEIGHT - 1;
			if ((((blockableBotLeftX <= livingTopRightX) && (livingTopRightX <= blockableBotRightX)) && ((blockableBotLeftY <= livingTopRightY) && (livingTopRightY <= blockableTopLeftY)))
				|| (((blockableBotLeftX <= livingTopLeftX) && (livingTopLeftX <= blockableBotRightX)) && ((blockableBotLeftY <= livingTopLeftY) && (livingTopLeftY <= blockableTopLeftY)))
				|| (((blockableBotLeftX <= livingBotRightX) && (livingBotRightX <= blockableBotRightX)) && ((blockableBotLeftY <= livingBotRightY) && (livingBotRightY <= blockableTopLeftY)))
				|| (((blockableBotLeftX <= livingBotLeftX) && (livingBotLeftX <= blockableBotRightX)) && ((blockableBotLeftY <= livingBotLeftY) && (livingBotLeftY <= blockableTopLeftY))))
			{
				return true;
			}
		}
	}

	return false;
}

bool StudentWorld::isOverlapping(double x, double y, double beingBurnedX, double beingBurnedY)
{
	//Check if overlapping
	if (calculateEuclidean(x, y, beingBurnedX, beingBurnedY) <= 100)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void StudentWorld::exited(double x, double y)
{
	//Determine if the citizens have exited
	list<Actor*>::iterator it;
	bool allExited = true;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it)->canExit() && isOverlapping(x, y, (*it)->getX(), (*it)->getY()))
		{
			(*it)->setToExited();
			increaseScore(500);
			(*it)->setToDead();
			(*it)->playSavedSound();
		}
		else if ((*it)->canExit() && !isOverlapping(x, y, (*it)->getX(), (*it)->getY()))
		{
			allExited = false;
		}

	}

	//If all the citizens have exited, and penelope is on top of the exit, she can exit
	if (allExited && isOverlapping(x, y, m_penelope->getX(), m_penelope->getY()))
	{
		m_penelope->setToExited();
	}
	return;
}

bool StudentWorld::checkIfOverlapping(double x, double y)
{
	//Iterate through each object and check if it's overlapping
	if (isOverlapping(x, y, m_penelope->getX(), m_penelope->getY()))
	{
		return true;
	}

	list<Actor*>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		if (isOverlapping(x, y, (*it)->getX(), (*it)->getY()))
		{
			return true;
		}

	}
	return false;
}

bool StudentWorld::checkIfAnythingBlocking(double x, double y, int pixelCheck, int dir)
{
	//For the direction the object is trying to move to, check if anything is blocking its path besides itself
	list<Actor*>::iterator it;
	switch (dir)
	{
	case 0:
		for (it = objects.begin(); it != objects.end(); it++)
		{
			if ((*it)->isAbleToBlock() && !isntBlocking(x, y, (*it)->getX(), (*it)->getY(), pixelCheck, dir) && (((*it)->getX() + pixelCheck) == x))
			{
			}
			else if ((*it)->isAbleToBlock() && !isntBlocking(x, y, (*it)->getX(), (*it)->getY(), pixelCheck, dir))
			{
				return true;
			}
		}
		break;
	case 1:
		for (it = objects.begin(); it != objects.end(); it++)
		{
			if ((*it)->isAbleToBlock() && !isntBlocking(x, y, (*it)->getX(), (*it)->getY(), pixelCheck, dir) && (((*it)->getY() + pixelCheck) == y))
			{
			}
			else if ((*it)->isAbleToBlock() && !isntBlocking(x, y, (*it)->getX(), (*it)->getY(), pixelCheck, dir))
			{
				return true;
			}
		}
		break;
	case 2:
		for (it = objects.begin(); it != objects.end(); it++)
		{
			if ((*it)->isAbleToBlock() && !isntBlocking(x, y, (*it)->getX(), (*it)->getY(), pixelCheck, dir) && (((*it)->getX() - pixelCheck) == x))
			{
			}
			else if ((*it)->isAbleToBlock() && !isntBlocking(x, y, (*it)->getX(), (*it)->getY(), pixelCheck, dir))
			{
				return true;
			}
		}
		break;
	case 3:
		for (it = objects.begin(); it != objects.end(); it++)
		{
			if ((*it)->isAbleToBlock() && !isntBlocking(x, y, (*it)->getX(), (*it)->getY(), pixelCheck, dir) && (((*it)->getY() - pixelCheck) == y))
			{
			}
			else if ((*it)->isAbleToBlock() && !isntBlocking(x, y, (*it)->getX(), (*it)->getY(), pixelCheck, dir))
			{
				return true;
			}
		}
		break;
	default:
		break;
	}
	return false;
}

void StudentWorld::damagedByFlame(double x, double y)
{
	//Check if player is killed by fire
	if (isOverlapping(x, y, m_penelope->getX(), m_penelope->getY()))
	{
		m_penelope->setToDead();
		m_penelope->playDeathSound();
		//End current level
	}

	//Check if any alive objects are burned
	list<Actor*>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it)->canBeBurned() && isOverlapping(x, y, (*it)->getX(), (*it)->getY()))
		{
			(*it)->setToDead();
			if ((*it)->isAbleToBlock())
			{
				(*it)->playDeathSound();
			}
			if ((*it)->canExplode())
			{
				createNewFlames((*it)->getX(), (*it)->getY());
			}
			(*it)->prepareNext();
		}
	}
}

void StudentWorld::fellIntoPit(double x, double y)
{
	//Check if any living object has fallen into the pit
	if (isOverlapping(x, y, m_penelope->getX(), m_penelope->getY()))
	{
		m_penelope->setToDead();
		m_penelope->playDeathSound();
	}

	list<Actor*>::iterator it;

	for (it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it)->canFallIntoPit() && isOverlapping(x, y, (*it)->getX(), (*it)->getY()))
		{
			(*it)->setToDead();
			(*it)->playDeathSound();
			(*it)->prepareNext();
		}
	}
}

void StudentWorld::infectedByVomit(double x, double y)
{
	//Check if any person has been infected by vomit
	if (isOverlapping(x, y, m_penelope->getX(), m_penelope->getY()))
	{
		m_penelope->setToInfected();
	}

	list<Actor*>::iterator it;

	for (it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it)->canBeInfected() && isOverlapping(x, y, (*it)->getX(), (*it)->getY()))
		{
			if (!(*it)->isInfected())
			{
				(*it)->playInfectedSound();
			}
			(*it)->setToInfected();
		}
	}
}

bool StudentWorld::gotGoods(double x, double y, Actor * goodie)
{
	//See if Penelope is overlapping the goodie
	if (isOverlapping(x, y, m_penelope->getX(), m_penelope->getY()))
	{
		increaseScore(50);
		goodie->setToDead();
		goodie->playDeathSound();
		return true;
	}
	return false;
}

void StudentWorld::createVaccineKit(double x, double y)
{
	//Create a new vaccine goodie
	objects.push_back(new VaccineGoodie(x / SPRITE_WIDTH, y / SPRITE_HEIGHT, this));
}

void StudentWorld::addVaccine()
{
	//Add a vaccine to penelope's kit
	m_penelope->addVaccine();
}

void StudentWorld::addCharges()
{
	//Add charges to the flamethrower
	m_penelope->addCharges();
}

void StudentWorld::addLandmine()
{
	//Add a landmine
	m_penelope->addLandmine();
}

void StudentWorld::plantLandmine(double x, double y)
{
	//Create a landmine
	objects.push_back(new Landmine(x, y, this));
}

void StudentWorld::landmineBoom(double x, double y, Actor* landmine)
{
	//See if Penelope is overlapping the landmine
	if (isOverlapping(x, y, m_penelope->getX(), m_penelope->getY()))
	{

		landmine->setToDead();
		landmine->playDeathSound();
		createNewFlames(x, y); //Center
		return;
	}

	//See if Citizen or Zombies are overlapping landmine
	list<Actor*>::iterator it;

	for (it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it)->canFallIntoPit() && isOverlapping(x, y, (*it)->getX(), (*it)->getY()))
		{
			landmine->setToDead();
			landmine->playDeathSound();
			createNewFlames(x, y); //Center
			return;
		}
	}

}

void StudentWorld::createNewFlames(double x, double y)
{
	//Create the surrounding flames for when a landmine explodes
	objects.push_front(new Flame(x, y, this));
	if (!wallIsBlocking(x - SPRITE_WIDTH, y + SPRITE_HEIGHT))
	{
		objects.push_front(new Flame(x - SPRITE_WIDTH, y + SPRITE_HEIGHT, this)); //NW
	}
	if (!wallIsBlocking(x, y + SPRITE_HEIGHT))
	{

		objects.push_front(new Flame(x, y + SPRITE_HEIGHT, this)); //N
	}
	if (!wallIsBlocking(x + SPRITE_WIDTH, y + SPRITE_HEIGHT))
	{

		objects.push_front(new Flame(x + SPRITE_WIDTH, y + SPRITE_HEIGHT, this)); //NE
	}
	if (!wallIsBlocking(x + SPRITE_WIDTH, y))
	{

		objects.push_front(new Flame(x + SPRITE_WIDTH, y, this)); //E
	}
	if (!wallIsBlocking(x + SPRITE_WIDTH, y - SPRITE_HEIGHT))
	{

		objects.push_front(new Flame(x + SPRITE_WIDTH, y - SPRITE_HEIGHT, this)); //SE
	}
	if (!wallIsBlocking(x, y - SPRITE_HEIGHT))
	{

		objects.push_front(new Flame(x, y - SPRITE_HEIGHT, this)); //S
	}
	if (!wallIsBlocking(x - SPRITE_WIDTH, y - SPRITE_HEIGHT))
	{

		objects.push_front(new Flame(x - SPRITE_WIDTH, y - SPRITE_HEIGHT, this)); //SW
	}
	if (!wallIsBlocking(x - SPRITE_WIDTH, y))
	{

		objects.push_front(new Flame(x - SPRITE_WIDTH, y, this)); //W
	}
	objects.push_front(new Pit(x / SPRITE_WIDTH, y / SPRITE_HEIGHT, this));
}

bool StudentWorld::shouldIVomit(double x, double y, int dir)
{
	double vomitX = x;
	double vomitY = y;
	switch (dir)
	{
	case 0: //Check right
		vomitX += SPRITE_WIDTH;
		break;
	case 1: //Check up
		vomitY += SPRITE_HEIGHT;
		break;
	case 2: //Check left
		vomitX -= SPRITE_WIDTH;
		break;
	case 3: //Check down
		vomitY -= SPRITE_HEIGHT;
		break;
	default:
		break;
	}

	//Determine if the vomit will land on Penelope or Citizen
	if (isOverlapping(vomitX, vomitY, m_penelope->getX(), m_penelope->getY()))
	{
		//Determine if the zombie actually vomits
		if (iVomited())
		{
			objects.push_back(new Vomit(vomitX, vomitY, this));
			playSound(SOUND_ZOMBIE_VOMIT);
			return true;
		}
	}

	list<Actor*>::iterator it;

	for (it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it)->canExit() && isOverlapping(vomitX, vomitY, (*it)->getX(), (*it)->getY()))
		{
			if (iVomited())
			{
				objects.push_back(new Vomit(vomitX, vomitY, this));
				playSound(SOUND_ZOMBIE_VOMIT);
				return true;
			}
		}
	}

	//If not, return false
	return false;
}

bool StudentWorld::iVomited()
{
	//Randomly determine if vomits
	int x = randInt(1, 3);
	if (x == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}


int StudentWorld::closestPerson(double x, double y)
{
	//Determine distance from zombie center to penelope
	double distance = calculateEuclidean(x, y, m_penelope->getX(), m_penelope->getY());
	double personX = m_penelope->getX();
	double personY = m_penelope->getY();

	//Determine if there is a closer distance to a citizen
	list<Actor*>::iterator it;

	for (it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it)->canExit())
		{
			double compareDistance = calculateEuclidean(x, y, (*it)->getX(), (*it)->getY());
			if (compareDistance < distance)
			{
				distance = compareDistance;
				personX = (*it)->getX();
				personY = (*it)->getY();
			}
		}
	}

	//If the Euclidean distance is greater than 6400, return a random direction
	if (distance > 6400)
	{
		int dir = randInt(0, 3);

		return dir;
	}
	//If not, determine the direction the zombie should go
	else
	{
		double distXtoP = x - personX;
		double distYtoP = y - personY;
		if ((distXtoP > 0) && (abs(distYtoP) == 0)) //Same row, zombie to right
		{
			return 2;

		}
		else if ((distXtoP < 0) && (abs(distYtoP) == 0)) //Same row, zombie to left
		{

			return 0;

		}
		else if ((abs(distXtoP) == 0) && (distYtoP > 0)) //Same col, zombie above
		{
			return 3;

		}
		else if ((abs(distXtoP) == 0) && (distYtoP < 0)) //Same col, zombie below
		{

			return 1;

		}
		else //Randomly determine a direction to move
		{
			bool noHor = false;
			bool noVert = false;
			int horOrVert = randInt(0, 1);
			if (horOrVert == 1) //horizontal
			{
				if (distXtoP < 0) //zombie to the left
				{

					return 0;
				}
				else if (distXtoP > 0) //zombie to the right
				{
					return 2;
				}
				noHor = true;
			}
			if (horOrVert == 0 || noHor) //vertical
			{
				if (distYtoP < 0) //zombie below
				{
					return 1;
				}
				else if (distYtoP > 0) //zombie above
				{
					return 3;
				}
				noVert = true;
			}
			if (noVert)
			{
				if (distXtoP < 0)
				{

					return 0;
				}
				else if (distXtoP > 0)
				{
					return 2;
				}
			}
		}
	}

	return 8;
}

void StudentWorld::createNewZombie(double x, double y)
{
	//Determine if the new zombie created is a smart zombie or dumb zombie
	int lol = randInt(1, 10);
	if (lol <= 3)
	{
		objects.push_back(new SmartZombie(x / SPRITE_WIDTH, y / SPRITE_HEIGHT, this));
	}
	else
	{
		objects.push_back(new DumbZombie(x / SPRITE_WIDTH, y / SPRITE_HEIGHT, this));
	}
}

int StudentWorld::moveCitizen(double x, double y)
{
	//Determine distance from Penelope
	double distXtoP = x - m_penelope->getX();
	double distYtoP = y - m_penelope->getY();
	double distanceP = calculateEuclidean(x, y, m_penelope->getX(), m_penelope->getY());

	//Set large arbitrary distance from zombie
	double distXtoZ = 500;
	double distYtoZ = 500;
	double distanceZ = calculateEuclidean(x, y, distXtoZ, distYtoZ);


	//Determine the closest zombie
	list<Actor*>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		if (!(*it)->canExit() && (*it)->canFallIntoPit())
		{
			double newDistance = calculateEuclidean(x, y, (*it)->getX(), (*it)->getY());

			//If the new distance is closer than the old, update it
			if (newDistance < distanceZ)
			{
				distanceZ = newDistance;
				distXtoZ = x - (*it)->getX();
				distYtoZ = y - (*it)->getY();
			}
		}
	}

	//if distance to Penelope < distance to Zombie and if the distance to Penelope is less than 80 pixels, find the direction to move in
	if ((distanceP < distanceZ) && (distanceP <= 6400))
	{
		if ((distXtoP > 0) && (abs(distYtoP) == 0)) //Same row, citizen to right
		{
			if (!checkIfThingIsBlocked(x - 2, y, 2, 2))
			{
				return 2;
			}
		}
		else if ((distXtoP < 0) && (abs(distYtoP) == 0)) //Same row, citizen to left
		{
			if (!checkIfThingIsBlocked(x + 2, y, 2, 0))
			{
				return 0;
			}
		}
		else if ((abs(distXtoP) == 0) && (distYtoP > 0)) //Same col, citizen above
		{
			if (!checkIfThingIsBlocked(x, y - 2, 2, 3))
			{
				return 3;
			}
		}
		else if ((abs(distXtoP) == 0) && (distYtoP < 0)) //Same col, citizen below
		{
			if (!checkIfThingIsBlocked(x, y + 2, 2, 1))
			{
				return 1;
			}
		}
		else //Randomly determine a direction to move if citizen is not in same row/col as penelope
		{
			bool noHor = false;
			bool noVert = false;
			int horOrVert = randInt(1, 2);
			if (horOrVert == 1) //move horizontally
			{
				if (distXtoP < 0) //citizen to the left
				{
					if (!checkIfThingIsBlocked(x + 2, y, 2, 0))
					{
						return 0;
					}

				}
				else if (distXtoP > 0) //citizen to the right
				{
					if (!checkIfThingIsBlocked(x - 2, y, 2, 2))
					{
						return 2;
					}
				}
				noHor = true;
			}
			if (horOrVert == 2 || noHor) //move vertically
			{
				if (distYtoP < 0) //citizen below
				{
					if (!checkIfThingIsBlocked(x, y + 2, 2, 1))
					{
						return 1;
					}
				}
				else if (distYtoP > 0) //citizen above
				{
					if (!checkIfThingIsBlocked(x, y - 2, 2, 3))
					{
						return 3;
					}
				}
				noVert = true;
			}
			if (noVert)
			{
				if (distXtoP < 0)
				{
					if (!checkIfThingIsBlocked(x + 2, y, 2, 0))
					{
						return 0;
					}

				}
				else if (distXtoP > 0)
				{
					if (!checkIfThingIsBlocked(x - 2, y, 2, 2))
					{
						return 2;
					}
				}
			}

		}
	}

	//Move away from zombie if the zombie is within 80 pixels
	else if (distanceZ <= 6400)
	{
		double distanceAway = distanceZ;
		double newDistance1 = 0;
		double newDistance2 = 0;
		double newDistance3 = 0;
		double newDistance4 = 0;
		double newDistance = 0;
		int dir = 8;
		int tempDir = 8;
		int dir0 = 0;
		int dir1 = 2;
		int dir2 = 1;
		int dir3 = 3;
		if (!checkIfThingIsBlocked(x + 2, y, 2, 0)) //check right
		{
			newDistance1 = closestZombie(x + 2, y);
		}
		if (!checkIfThingIsBlocked(x - 2, y, 2, 2)) //check left
		{
			newDistance2 = closestZombie(x - 2, y);
		}
		if (!checkIfThingIsBlocked(x, y + 2, 2, 1)) //check up
		{
			newDistance3 = closestZombie(x, y + 2);
		}
		if (!checkIfThingIsBlocked(x, y - 2, 2, 3)) //check down
		{
			newDistance4 = closestZombie(x, y - 2);
		}
		double arr[4] = { newDistance1, newDistance2, newDistance3, newDistance4 };
		int direct[4] = { dir0, dir1, dir2, dir3 };

		//Determine the furthest distance from the closest Zombie
		for (int x = 0; x < 4; x++)
		{
			if (arr[x] > newDistance)
			{
				newDistance = arr[x];
				tempDir = direct[x];
			}

		}

		//If the new distance is further than the current distance, return the direction it should go
		if (newDistance > distanceAway)
		{
			dir = tempDir;
		}
		return dir;
	}

	//Do nothing for the tick
	return 8;
}

bool StudentWorld::checkIfThingIsBlocked(double x, double y, int pixelCheck, int dir)
{
	//See if Penelope is blocking the citizen
	if (!isntBlocking(x, y, m_penelope->getX(), m_penelope->getY(), pixelCheck, dir))
	{
		return true;
	}

	//See if anything is blocking the citizen
	if (checkIfAnythingBlocking(x, y, pixelCheck, dir))
	{
		return true;
	}
	return false;
};

double StudentWorld::calculateEuclidean(double x1, double y1, double x2, double y2)
{
	//Calculate the Euclidean using the center values of the objects passed
	double x1Cent = x1 + ((SPRITE_WIDTH - 1) / 2);
	double y1Cent = y1 + ((SPRITE_HEIGHT - 1) / 2);
	double x2Cent = x2 + ((SPRITE_WIDTH - 1) / 2);
	double y2Cent = y2 + ((SPRITE_HEIGHT - 1) / 2);
	double x;
	x = pow((x2Cent - x1Cent), 2) + pow((y2Cent - y1Cent), 2);
	return x;
}

double StudentWorld::closestZombie(double x, double y)
{
	//Determine the distance to the closest zombie
	double distance = 680680;
	double newDistance = 680681;
	list<Actor*>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		if (!(*it)->canExit() && (*it)->canFallIntoPit())
		{
			newDistance = calculateEuclidean(x, y, (*it)->getX(), (*it)->getY());
			if (newDistance < distance)
			{
				distance = newDistance;
			}
		}
	}
	return distance;
}

void StudentWorld::addFlame(double x, double y)
{
	//Determine the flames that are able to be created from player's flamethrower
	int dir = m_penelope->getDirection();
	double pos1x = x;
	double pos1y = y;
	double pos2x = x;
	double pos2y = y;
	double pos3x = x;
	double pos3y = y;

	switch (dir)
	{
	case 0:
		pos1x = calculateNewFlames(x, y, dir);
		pos2x = calculateNewFlames(pos1x, y, dir);
		pos3x = calculateNewFlames(pos2x, y, dir);
		break;
	case 180:
		pos1x = calculateNewFlames(x, y, dir);
		pos2x = calculateNewFlames(pos1x, y, dir);
		pos3x = calculateNewFlames(pos2x, y, dir);
		break;
	case 90:
		pos1y = calculateNewFlames(x, y, dir);
		pos2y = calculateNewFlames(x, pos1y, dir);
		pos3y = calculateNewFlames(x, pos2y, dir);
		break;
	case 270:
		pos1y = calculateNewFlames(x, y, dir);
		pos2y = calculateNewFlames(x, pos1y, dir);
		pos3y = calculateNewFlames(x, pos2y, dir);
		break;
	default:
		return;
	}

	//Determine if all the flames are able to be created (not blocked)
	if (!checkIfOverlappingFlamethrower(pos1x, pos1y))
	{
		objects.push_back(new Flame(pos1x, pos1y, this));

		if (!checkIfOverlappingFlamethrower(pos2x, pos2y))
		{
			objects.push_back(new Flame(pos2x, pos2y, this));

			if (!checkIfOverlappingFlamethrower(pos3x, pos3y))
			{
				objects.push_back(new Flame(pos3x, pos3y, this));
			}
		}
	}


}

double StudentWorld::calculateNewFlames(double x, double y, int dir)
{
	//Help determine the coordinates for the new flames being created by the flamethrower
	switch (dir)
	{
	case 0:
		return x + SPRITE_WIDTH;
	case 180:
		return x - SPRITE_WIDTH;
	case 90:
		return y + SPRITE_HEIGHT;
	case 270:
		return y - SPRITE_HEIGHT;
	default:
		return 0;
	}
}

bool StudentWorld::checkIfOverlappingFlamethrower(double x, double y)
{
	//Check if the flames created by the flamethrower overlap an exit or wall
	list<Actor*>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it)->canOverlapFlamethrower())
		{
			if (isOverlapping(x, y, (*it)->getX(), (*it)->getY()))
			{
				return true;
			}
		}
	}
	return false;
}