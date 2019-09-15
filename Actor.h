#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"
#include "StudentWorld.h"
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, int startDirection,
		  int depth, StudentWorld* sw, bool isAbleToBlock, bool alive, bool canBeBurned); //Constructor
	virtual ~Actor(); //Destructor

	virtual void doSomething() //Virtual doSomething
	{
		return;
	}
	virtual void playDeathSound() //Play unique death sound
	{
		return;
	}
	virtual void playInfectedSound() //Play uniquie infected sound
	{
		return;
	}
	virtual void playSavedSound() //Play unique saved sound
	{
		return;
	}
	virtual void prepareNext() //After death, do anything new if required
	{
		return;
	}
	virtual bool canDie() //See if object can die
	{
		return true;
	}
	virtual bool canExit() //Determine objects that can exit
	{
		return false;
	}
	virtual bool hasExited() //Determine if an object has exited
	{
		return false;
	}
	virtual void setToExited() //Sets exit status to true when possible
	{
		return;
	}
	virtual bool canFallIntoPit() //Determine objects that can fall into a pit
	{
		return false;
	}
	virtual bool canBeInfected() //Determine objects that can be infected
	{
		return false;
	}
	virtual bool canExplode() //Determine objects that can explode
	{
		return false;
	}
	virtual bool canOverlapFlamethrower() //Determine objects that cannot overlap with the flamethrower
	{
		return false;
	}
	virtual void setToInfected() //Sets infection status to true when possible
	{
		return;
	}
	virtual bool isInfected()
	{
		return false;
	}

	bool isAbleToBlock(); //Check if object has blocking capabilities
	bool canBeBurned(); //Check if object has burning capabilities
	void setToDead(); //Set alive status to false
	bool isAlive(); //Returns living status



	StudentWorld* getWorld() //Return the associated StudentWorld
	{
		return m_studentWorld;
	}

private:
	StudentWorld * m_studentWorld;
	bool m_isAbleToBlock; //Can block something
	bool m_canDie; //Can be alive or dead
	bool m_alive; //Living status
	bool m_canBeBurned; //Can be burned by a flame
};



class Citizen : public Actor
{
public:
	Citizen(double level_x, double level_y, StudentWorld* sw);
	virtual void doSomething();
	virtual void playDeathSound();
	virtual void prepareNext();
	virtual bool canExit();
	virtual void setToExited();
	virtual bool hasExited();
	virtual bool canFallIntoPit();
	virtual bool canBeInfected();
	virtual void playSavedSound();
	virtual void playInfectedSound();
	virtual void setToInfected(); //Set infected for Player and Citizen
	virtual bool isInfected(); //Return the infection status
private:
	bool m_infection; //Infection status
	int m_infectionCount; //Infection count
	bool m_paralyzed; //Paralyzation for the tick
	bool m_exited; //Has citizen exited
};



class Exit : public Actor
{
public:
	Exit(double level_x, double level_y, StudentWorld* sw);
	virtual void doSomething();
	virtual bool canDie();
	virtual bool canOverlapFlamethrower();
};



class Flame : public Actor
{
public:
	Flame(double x, double y, StudentWorld* sw);
	virtual void doSomething();
private:
	int m_tickCount; //Flame's lifespan
};



class GasCanGoodie : public Actor
{
public:
	GasCanGoodie(double level_x, double level_y, StudentWorld* sw);
	virtual void doSomething();
	virtual void playDeathSound();
private:
};



class Landmine : public Actor
{
public:
	Landmine(double level_x, double level_y, StudentWorld* sw);
	virtual void doSomething();
	virtual bool canExplode();
	virtual void playDeathSound();
private:
	int m_safetyTicks; //Ticks before detonation
	bool m_active; //Marks if the landmine is active
};



class LandmineGoodie : public Actor
{
public:
	LandmineGoodie(double level_x, double level_y, StudentWorld* sw);
	virtual void doSomething();
	virtual void playDeathSound();

private:
};



class Pit : public Actor
{
public:
	Pit(double level_x, double level_y, StudentWorld* sw);
	virtual void doSomething();
	virtual bool canDie();
};



class Penelope : public Actor
{
public:
	Penelope(double level_x, double level_y, StudentWorld* sw);
	virtual void doSomething();
	virtual void playDeathSound();
	virtual bool canExit();
	virtual void setToExited();
	virtual bool hasExited();
	virtual bool canFallIntoPit();
	virtual bool canBeInfected();
	virtual void setToInfected(); //Set infected for Player and Citizen
	virtual bool isInfected(); //returns infection status

	int getFlames(); //Returns number of charges
	int getVaccines(); //Returns number of vaccines
	int getInfected(); //Returns number of infections
	int getMines(); //Returns number of landmines
	void addVaccine(); //Adds a vaccine
	void addCharges(); //Adds 5 charges
	void addLandmine(); //Adds 2 landmines
	

private:
	int m_landmines; //Number of landmines
	int m_charges; //Number of flamethrower charges
	int m_vaccines; //Number of vaccines
	int m_infectionCount; //Number of infections
	bool m_infection; //Infection status
	bool m_exited; //Exit status
};



class VaccineGoodie : public Actor
{
public:
	VaccineGoodie(double level_x, double level_y, StudentWorld* sw);
	virtual void doSomething();
	virtual void playDeathSound();
private:
};



class Vomit : public Actor
{
public:
	Vomit(double x, double y, StudentWorld* sw);
	virtual void doSomething();
private:
	int m_tickCount; //Living count
};



class Wall : public Actor
{
public:
	Wall(double level_x, double level_y, StudentWorld* sw);
	virtual bool canDie();
	virtual bool canOverlapFlamethrower();
};



class DumbZombie : public Actor
{
public:
	DumbZombie(double level_x, double level_y, StudentWorld* sw);
	virtual void doSomething();
	virtual void playDeathSound();
	virtual void prepareNext();
	virtual bool canFallIntoPit();
private:
	int m_movementPlan; //Values in movement plan
	bool m_paralyzed; //Paralyzation status
	bool m_hasVaccine; //Status if holding a vaccine or not
};



class SmartZombie : public Actor
{
public:
	SmartZombie(double level_x, double level_y, StudentWorld* sw);
	virtual void doSomething();
	virtual void playDeathSound();
	virtual void prepareNext();
	virtual bool canFallIntoPit();
private:
	int m_movementPlan; //Values in movement plan
	bool m_paralyzed; //Paralyzation status
};

#endif // ACTOR_H_
