#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>
#include <iostream>

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Penelope;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	virtual ~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	bool checkIfAnythingBlocking(double x, double y, int pixelCheck, int dir); //Checks if object that can block, except penelope and the object itself, is blocking
	bool isntBlocking(double firstX, double firstY, double x, double y, int pixelCheck, int dir); //Check if the object passed in is blocking
																								  //Return true if something is blocking
	bool wallIsBlocking(double firstX, double firstY); //Check if the object passed in is blocked by a wall;
	bool isOverlapping(double x, double y, double beingBurnedX, double beingBurnedY); //Check for overlapping
	bool checkIfOverlapping(double x, double y); //Checks if any object is overlapping
	void exited(double x, double y); //Determine if the level is completed and/or any people have exited
	void damagedByFlame(double x, double y); //Kill object if overlapping with flame
	void fellIntoPit(double x, double y); //Kill object if it fell into a pit
	void infectedByVomit(double x, double y); //Check who got infected by the vomit
	bool gotGoods(double x, double y, Actor * goodie); //Check if Penelope picked up any goodie
	void createVaccineKit(double x, double y); //Creates a vaccine kit from dead dumb zombie
	void addVaccine(); //Add vaccine to Penelope's kit
	void addCharges(); //Add charges to Penelope's kit
	void addLandmine(); //Add landmine to Penelope's kit
	void plantLandmine(double x, double y); //Create a landmine
	void landmineBoom(double x, double y, Actor* landmine); //Check if any living object overlapped with a landmine
	void createNewFlames(double x, double y); //Create flames after a landmine explodes
	bool shouldIVomit(double x, double y, int dir); //Check if the zombie should vomit
	bool iVomited(); //Have the zombie produce a vomit
	int closestPerson(double x, double y); //Determine the direction to the closest person
	void createNewZombie(double x, double y); //Create a new zombie from a dead citizen
	int moveCitizen(double x, double y); //Moves the citizen either closer to player or further from zombie
	bool checkIfThingIsBlocked(double x, double y, int pixelCheck, int dir); //Checks if any wall, person, or zombie is blocking
	double calculateEuclidean(double x1, double y1, double x2, double y2); //Takes in two object coordinates and calculates the Euclidean distance from their centers
	double closestZombie(double x, double y); //Determines the closest zombie and returns its distance
	void addFlame(double x, double y); //Creates a new flame
	double calculateNewFlames(double x, double y, int dir); //Helps calculate new flame coordinates
	bool checkIfOverlappingFlamethrower(double x, double y); //Checks if the flames overlap with a wall or exit
	bool levelComplete(); //Checks if the level is complete

private:
	Penelope * m_penelope;
	list<Actor*> objects;
};

#endif // STUDENTWORLD_H_
