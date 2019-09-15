#include "Actor.h"
#include "Level.h"
#include <cmath>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(int imageID, double startX, double startY, int startDirection,
	int depth, StudentWorld* sw, bool isAbleToBlock, bool alive, bool canBeBurned)
	: GraphObject(imageID, startX, startY, startDirection, depth), m_studentWorld(sw)
{
	m_isAbleToBlock = isAbleToBlock;
	m_alive = alive;
	m_canBeBurned = canBeBurned;
};

Actor::~Actor()
{
};

bool Actor::isAbleToBlock()
{
	return m_isAbleToBlock;
}

bool Actor::canBeBurned()
{
	return m_canBeBurned;
}

bool Actor::isAlive()
{
	return m_alive;
}

void Actor::setToDead()
{
	m_alive = false;
}



Citizen::Citizen(double level_x, double level_y, StudentWorld* sw) //Constructor for Exit
	: Actor(IID_CITIZEN, SPRITE_WIDTH * level_x, SPRITE_HEIGHT * level_y, right, 0, sw, true, true, true)
{
	m_infection = false;
	m_infectionCount = 0;
	m_paralyzed = false;
	m_exited = false;
}

void Citizen::doSomething() //Bounding box must never intersect with another citizen, penelope, dumb or smart zombie
{
	//End if dead
	if (!isAlive())
	{
		return;
	}

	//Increment the infection count
	if (isInfected())
	{
		m_infectionCount += 1;

		//Kill the citizen and create the zombie if completely infected
		if (m_infectionCount == 500)
		{
			setToDead();
			getWorld()->playSound(SOUND_ZOMBIE_BORN);
			getWorld()->increaseScore(-1000);
			getWorld()->createNewZombie(getX(), getY());
			return;
		}
	}

	//Alternative turns for paralyzation
	if (m_paralyzed)
	{
		m_paralyzed = false;
		return;
	}
	m_paralyzed = true;

	//Set the direction for the citizen to potentially move to
	double newX = getX();
	double newY = getY();
	int dir = getWorld()->moveCitizen(getX(), getY());

	switch (dir)
	{
	case 0: //right
		setDirection(right);
		newX += 2;
		moveTo(newX, newY);
		return;
		break;
	case 1: //up
		setDirection(up);
		newY += 2;
		moveTo(newX, newY);
		return;
		break;
	case 2: //left
		setDirection(left);
		newX -= 2;
		moveTo(newX, newY);
		return;
		break;
	case 3: //down
		setDirection(down);
		newY -= 2;
		moveTo(newX, newY);
		return;
		break;
	case 8:
		return;
	default:
		return;
		break;
	}
}

void Citizen::playDeathSound()
{
	//Play SOUND_CITIZEN_DIE
	getWorld()->playSound(SOUND_CITIZEN_DIE);
}

void Citizen::prepareNext()
{
	//Decrease Score by 1000
	getWorld()->increaseScore(-1000);
}

bool Citizen::canExit()
{
	return true;
}

void Citizen::setToExited()
{
	m_exited = true;
}

bool Citizen::hasExited()
{
	return m_exited;
}

bool Citizen::canFallIntoPit()
{
	return true;
}

bool Citizen::canBeInfected()
{
	return true;
}

void Citizen::playInfectedSound()
{
	getWorld()->playSound(SOUND_CITIZEN_INFECTED);
}

bool Citizen::isInfected()
{
	return m_infection;
}

void Citizen::playSavedSound()
{
	getWorld()->playSound(SOUND_CITIZEN_SAVED);
}

void Citizen::setToInfected()
{
	m_infection = true;
}




Exit::Exit(double level_x, double level_y, StudentWorld* sw) //Constructor for Exit
	: Actor(IID_EXIT, SPRITE_WIDTH * level_x, SPRITE_HEIGHT * level_y, right, 1, sw, false, false, false)
{
}

void Exit::doSomething()
{

	//Set all the overlapping citizens and/or player to exit the game
	getWorld()->exited(getX(), getY());
}

bool Exit::canDie() //See if object can die
{
	return false;
}

bool Exit::canOverlapFlamethrower()
{
	return true;
}



Flame::Flame(double x, double y, StudentWorld* sw) //Constructor for Exit
	: Actor(IID_FLAME, x, y, up, 0, sw, false, true, false)
{
	m_tickCount = 0;
}

void Flame::doSomething()
{
	if (!isAlive()) //End if the flame is dead
	{
		return;
	}

	if (m_tickCount == 2) //Flame is dead after two ticks, doing nothing more
	{
		setToDead();
		return;
	}

	++m_tickCount; //Increment the number of ticks

	getWorld()->damagedByFlame(getX(), getY()); //Check if any objects are damaged by the flame
}

GasCanGoodie::GasCanGoodie(double level_x, double level_y, StudentWorld* sw) //Constructor for Exit
	: Actor(IID_GAS_CAN_GOODIE, SPRITE_WIDTH * level_x, SPRITE_HEIGHT * level_y, right, 1, sw, false, true, true)
{
}

void GasCanGoodie::doSomething()
{
	//Check if vaccine is alive
	if (!isAlive())
	{
		return;
	}

	//Add a charges if picked up
	if (getWorld()->gotGoods(getX(), getY(), this))
	{
		getWorld()->addCharges();
	}
}

void GasCanGoodie::playDeathSound() //Play unique death sound
{
	getWorld()->playSound(SOUND_GOT_GOODIE);
}



Landmine::Landmine(double level_x, double level_y, StudentWorld* sw) //Constructor for Exit
	: Actor(IID_LANDMINE, level_x, level_y, right, 1, sw, false, true, true)
{
	m_safetyTicks = 30;
	m_active = false;
}

void Landmine::doSomething() //Can be damaged by flame. If flame overlaps with landmine, landmine should be triggered as if person stepped on it
{
	//Check if alive
	if (!isAlive())
	{
		return;
	}

	//Check if Active
	if (!m_active)
	{
		m_safetyTicks--;
		if (m_safetyTicks == 0)
		{
			m_active = true;
		}
		return;
	}

	//Explode if active
	getWorld()->landmineBoom(getX(), getY(), this);
}

void Landmine::playDeathSound()
{
	//Play SOUND_LANDMINE_EXPLODE
	getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
}

bool Landmine::canExplode()
{
	return true;
}

LandmineGoodie::LandmineGoodie(double level_x, double level_y, StudentWorld* sw) //Constructor for Exit
	: Actor(IID_LANDMINE_GOODIE, SPRITE_WIDTH * level_x, SPRITE_HEIGHT * level_y, right, 1, sw, false, true, true)
{
}

void LandmineGoodie::doSomething()
{

	//Check if vaccine is alive
	if (!isAlive())
	{
		return;
	}

	//Add a landmine if picked up
	if (getWorld()->gotGoods(getX(), getY(), this))
	{
		getWorld()->addLandmine();
	}
}

void LandmineGoodie::playDeathSound() //Play unique death sound
{
	getWorld()->playSound(SOUND_GOT_GOODIE);
}



Penelope::Penelope(double level_x, double level_y, StudentWorld* sw) //Penelope's constructor
	: Actor(IID_PLAYER, SPRITE_WIDTH * level_x, SPRITE_HEIGHT * level_y, right, 0, sw, true, true, true)
{
	m_charges = 0;
	m_infection = false;
	m_infectionCount = 0;
	m_landmines = 0;
	m_vaccines = 0;
	m_exited = false;
};

void Penelope::doSomething()
{

	//End if dead
	if (!isAlive())
	{
		return;
	}

	//If player is infected, increase the infection count and if the infection count is 500, then the player has died
	if (isInfected())
	{
		m_infectionCount += 1;
		if (m_infectionCount == 500)
		{
			setToDead();
			getWorld()->playSound(SOUND_PLAYER_DIE);
			return;
		}
	}

	//Receive the input key from the user
	int ch = 0;
	double newX = getX();
	double newY = getY();

	if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
			//Directional inputs
		case KEY_PRESS_UP:
			setDirection(up);

			if (!getWorld()->checkIfAnythingBlocking(newX, newY, 4, 1))
			{
				newY += 4;
				moveTo(newX, newY);
			}
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);

			if (!getWorld()->checkIfAnythingBlocking(newX, newY, 4, 3))
			{
				newY -= 4;
				moveTo(newX, newY);
			}
			break;
		case KEY_PRESS_LEFT:
			setDirection(left);

			if (!getWorld()->checkIfAnythingBlocking(newX, newY, 4, 2))
			{
				newX -= 4;
				moveTo(newX, newY);
			}
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);

			if (!getWorld()->checkIfAnythingBlocking(newX, newY, 4, 0))
			{
				newX += 4;
				moveTo(newX, newY);
			}
			break;
			//Vaccinate the player. If the player is not infected, they still lose a vaccine
		case KEY_PRESS_ENTER:
			if (m_vaccines >= 1)
			{
				m_infection = false;
				m_infectionCount = 0;
				m_vaccines--;
			}
			break;
			//Use a flamethrower charge
		case KEY_PRESS_SPACE:
			if (m_charges >= 1)
			{
				m_charges--;
				getWorld()->playSound(SOUND_PLAYER_FIRE);
				getWorld()->addFlame(getX(), getY());
			}
			break;
			//Plant a landmine
		case KEY_PRESS_TAB:
			if (m_landmines >= 1)
			{
				getWorld()->plantLandmine(getX(), getY());
				m_landmines--;
			}
			break;
		}
	}
};

void Penelope::playDeathSound()
{
	//Play SOUND_PLAYER_DIE
	getWorld()->playSound(SOUND_PLAYER_DIE);
}

bool Penelope::canExit()
{
	return true;
}

void Penelope::setToExited()
{
	m_exited = true;
}

bool Penelope::hasExited()
{
	return m_exited;
}

bool Penelope::canFallIntoPit()
{
	return true;
}

bool Penelope::canBeInfected()
{
	return true;
}

void Penelope::addVaccine()
{
	m_vaccines += 1;
}

void Penelope::addCharges()
{
	m_charges += 5;
}

void Penelope::addLandmine()
{
	m_landmines += 2;
}

bool Penelope::isInfected()
{
	return m_infection;
}

int Penelope::getVaccines()
{
	return m_vaccines;
}
int Penelope::getMines()
{
	return m_landmines;
}
int Penelope::getInfected()
{
	return m_infectionCount;
}

int Penelope::getFlames()
{
	return m_charges;
}

void Penelope::setToInfected()
{
	m_infection = true;
}



Pit::Pit(double level_x, double level_y, StudentWorld* sw) //Constructor for Pit
	: Actor(IID_PIT, SPRITE_WIDTH * level_x, SPRITE_HEIGHT * level_y, right, 0, sw, false, false, false)
{
}

void Pit::doSomething()
{

	getWorld()->fellIntoPit(getX(), getY());
}

bool Pit::canDie()
{
	return false;
}



VaccineGoodie::VaccineGoodie(double level_x, double level_y, StudentWorld* sw) //Constructor for VaccineGoodie
	: Actor(IID_VACCINE_GOODIE, SPRITE_WIDTH * level_x, SPRITE_HEIGHT * level_y, right, 0, sw, false, true, true)
{
}

void VaccineGoodie::doSomething()
{
	//Check if vaccine is alive
	if (!isAlive())
	{
		return;
	}

	//Add a vaccine if picked up
	if (getWorld()->gotGoods(getX(), getY(), this))
	{
		getWorld()->addVaccine();
	}
}

void VaccineGoodie::playDeathSound() //Play unique death sound
{
	getWorld()->playSound(SOUND_GOT_GOODIE);
}


Vomit::Vomit(double x, double y, StudentWorld* sw) //Constructor for Wall
	: Actor(IID_VOMIT, x, y, 0, 0, sw, false, true, false)
{
	m_tickCount = 0;
}

void Vomit::doSomething()
{
	//End if dead
	if (!isAlive())
	{
		return;
	}

	//The vomit is dead after two ticks
	if (m_tickCount == 2)
	{
		setToDead();
		return;
	}
	m_tickCount++;

	//Check if anything gets infected by the vomit
	getWorld()->infectedByVomit(getX(), getY());
}



Wall::Wall(double level_x, double level_y, StudentWorld* sw) //Constructor for Wall
	: Actor(IID_WALL, SPRITE_WIDTH * level_x, SPRITE_HEIGHT * level_y, right, 0, sw, true, false, false)
{
}

bool Wall::canDie() //See if object can die
{
	return false;
}

bool Wall::canOverlapFlamethrower()
{
	return true;
}



DumbZombie::DumbZombie(double level_x, double level_y, StudentWorld* sw) //Constructor for Wall
	: Actor(IID_ZOMBIE, SPRITE_WIDTH * level_x, SPRITE_HEIGHT * level_y, right, 0, sw, true, true, true)
{
	m_movementPlan = 0;
	m_paralyzed = false;
	int x = randInt(1, 10);
	if (x == 1)
	{
		m_hasVaccine = true;
	}
	else
	{
		m_hasVaccine = false;
	}
}

void DumbZombie::doSomething()
{
	//Check if dead
	if (!isAlive())
	{
		return;
	}

	//Alternative turns for paralyzation
	if (m_paralyzed)
	{
		m_paralyzed = false;
		return;
	}
	m_paralyzed = true;

	//See if the zombie vomits and if it does, end zombie actions for the turn
	if (getWorld()->shouldIVomit(getX(), getY(), getDirection() / 90))
	{
		return;
	}

	//Determine new movement plan and set a random direction
	int dir = getDirection() / 90;
	double newX = getX();
	double newY = getY();
	if (m_movementPlan == 0)
	{
		m_movementPlan = randInt(3, 10);
		dir = randInt(0, 3);
	}

	switch (dir)
	{
	case 0:
		newX += 1;
		break;
	case 1:
		newY += 1;
		break;
	case 2:
		newX -= 1;
		break;
	case 3:
		newY -= 1;
		break;
	default:
		break;
	}

	//If the new direction chosen is blocked, set the movement plan to 0
	if (!getWorld()->checkIfThingIsBlocked(getX(), getY(), 1, dir))
	{
		setDirection(dir * 90);
		moveTo(newX, newY);
		m_movementPlan--;
	}
	//If it's not blocked, move the zombie in the specified direction
	else
	{
		m_movementPlan = 0;
	}
}

void DumbZombie::playDeathSound()
{
	//Play SOUND_ZOMBIE_DIE
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
}

void DumbZombie::prepareNext()
{
	getWorld()->increaseScore(1000);

	//If the zombie is holding a vaccine...
	if (m_hasVaccine)
	{
		//Determine where the vaccine kit will be thrown
		double newX = getX();
		double newY = getY();
		int dir = randInt(0, 3);
		switch (dir)
		{
		case 0:
			dir = right;
			newX += SPRITE_WIDTH;
			break;
		case 1:
			dir = up;
			newY += SPRITE_HEIGHT;
			break;
		case 2:
			dir = left;
			newX -= SPRITE_WIDTH;
			break;
		case 3:
			dir = down;
			newY -= SPRITE_HEIGHT;
			break;
		default:
			break;
		}

		//If the new vaccine kit location is not overlapping anything, create it
		if (!getWorld()->checkIfOverlapping(newX, newY))
		{
			getWorld()->createVaccineKit(newX, newY);
		}
	}
}

bool DumbZombie::canFallIntoPit()
{
	return true;
}



SmartZombie::SmartZombie(double level_x, double level_y, StudentWorld* sw) //Constructor for Wall
	: Actor(IID_ZOMBIE, SPRITE_WIDTH * level_x, SPRITE_HEIGHT * level_y, right, 0, sw, true, true, true)
{
	m_movementPlan = 0;
	m_paralyzed = false;
}

void SmartZombie::doSomething()
{
	//Check if dead
	if (!isAlive())
	{
		return;
	}

	//Alternative turns for paralyzation
	if (m_paralyzed)
	{
		m_paralyzed = false;
		return;
	}
	m_paralyzed = true;

	//See if the zombie vomits and if it does, end zombie actions for the turn
	if (getWorld()->shouldIVomit(getX(), getY(), getDirection() / 90))
	{
		return;
	}

	//Determine new movement plan and set a random direction
	int dir = getDirection() / 90;
	double newX = getX();
	double newY = getY();
	if (m_movementPlan == 0)
	{
		m_movementPlan = randInt(3, 10);
		dir = getWorld()->closestPerson(getX(), getY());

	}

	switch (dir)
	{
	case 0:
		newX += 1;
		break;
	case 1:
		newY += 1;
		break;
	case 2:
		newX -= 1;
		break;
	case 3:
		newY -= 1;
		break;
	case 8:
		return;
	default:
		return;
		break;
	}


	//If the new direction chosen is blocked, set the movement plan to 0
	if (!getWorld()->checkIfThingIsBlocked(getX(), getY(), 1, dir))
	{

		setDirection(dir * 90);
		moveTo(newX, newY);
		m_movementPlan--;
	}
	//If it's not blocked, move the zombie in the specified direction
	else
	{

		m_movementPlan = 0;
	}
}

void SmartZombie::prepareNext()
{
	getWorld()->increaseScore(2000);
}

void SmartZombie::playDeathSound()
{
	//Play SOUND_ZOMBIE_DIE
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
}

bool SmartZombie::canFallIntoPit()
{
	return true;
}