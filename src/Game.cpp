#include "Game.h"

#include <cstdio>
#include "Settings.h"

Game::Game() {
	bikes.clear();
	for (int i = 0; i < bikesNum; i++) {
		Bike *bike = new Bike();
		bikes.push_back(bike);
	}
	secondsToNextPhysicsTick = 0;
}

Game::~Game() {
}

void Game::newGame() {
	printf("Game restart\n");
	for (int i = 0; i < bikesNum; i++) {
		Bike *bike = getBike(i);
		// TODO better bike initialization
		bike->pos.x = mapSizeX * (i+1) / (bikesNum+1);
		bike->pos.z = mapSizeZ - bikeRadius;
		bike->direction = 0;
		bike->speed = defaultBikeSpeed;
		bike->wallHeight = 1;
		bike->resetWalls();
		bike->setColor((i+1)   %2,
		              ((i+1)/2)%2,
		              ((i+1)/4)%2);
	}
}

bool Game::onFrame(float frameSec) {
	secondsToNextPhysicsTick -= frameSec;
	while (secondsToNextPhysicsTick < 0) physicsTick();
	return testForGameOver();
}

int Game::bikesInGame() {
	return bikes.size();
}

Bike *Game::getBike(int bikeID) {
	return bikes.at(bikeID);
}

int Game::nextLivingBike(int start, bool next) {
	int tries = 0; // loop would be infinite if all bikes are dead, but game should have ended then
	int newBikeID = start;
	do {
		if (tries++ > bikesInGame()) {
			printf("ERROR nextLivingBike got into an infinite loop\n");
			return start;
		}
		newBikeID += next ? 1 : bikesInGame()-1;
		newBikeID %= bikesInGame();
	} while (getBike(newBikeID)->isDead());
	return newBikeID;
}

bool Game::collideBikeWithEverything(Bike *bike) {
	if (bike->collideWithMapBorder())
		return true;
	for (int i = 0; i < bikesInGame(); i++) {
		Bike *otherBike = getBike(i);
		if (otherBike->isDead()) continue;
		if (bike->collideWithWalls(otherBike))
			return true;
		if (otherBike != bike && bike->collideWithBike(otherBike))
			return true;
	}
	return false;
}

void Game::collideAllBikes() {
	for (int i = 0; i < bikesInGame(); i++)
		if (collideBikeWithEverything(getBike(i)))
			killBike(i);
}

void Game::killBike(int bikeID) {
	Bike *bike = getBike(bikeID);
	if (bike->isDying()) return;
	bike->wallHeight = 0.9999;
	printf("Bike %i crashed\n", bikeID);
}

bool Game::testForGameOver() {
	int bikesLeft = 0;
	int lastLivingBike = -1;
	for (int i = 0; i < bikesInGame(); i++) {
		if (!getBike(i)->isDying()) {
			bikesLeft++;
			if (bikesLeft >= 2) return false ;
			lastLivingBike = i;
		}
	}
	// assume bikesLeft < 2
	if (lastLivingBike == -1) printf("Tie!\n");
	else printf("Bike %i wins!\n", lastLivingBike);
	return true;
}

void Game::aiTick() {
	// TODO control the bots
	// start at 2, because 0 and 1 are the players
	for (int i = 2; i < bikesInGame(); i++) {
		Bike *bike = getBike(i);
		if (bike->isDying()) continue;
		Bike *ghostA = new Bike(bike);
		bike->wallHeight = 0; // simulate death to be ignored on collision tests
		bool right, turn = false;
		// move twice to avoid frontal crash with other bike
		ghostA->onPhysicsTick();
		ghostA->onPhysicsTick();
		if (collideBikeWithEverything(ghostA) || ghostA->collideWithWalls(ghostA)) {
			// bike is about to crash, but could dodge
			turn = true;
			right = true;
			// TODO fix turning to random direction
//			right = random()%2 == 0; // 1:1 left or right
//			// is it safe to move into this direction?
//			Bike *ghostB = new Bike(bike);
//			ghostB->turn(right);
//			ghostB->onPhysicsTick();
//			if (collideBikeWithEverything(ghostB) || ghostB->collideWithWalls(ghostB)) {
//				// no, it's not safe this way, turn the other way
//				right = !right;
//			}
//			delete ghostB;
		}
		delete ghostA;
		bike->wallHeight = 1;
		if (turn) bike->turn(right);
	}
}

void Game::physicsTick() {
	secondsToNextPhysicsTick += physicsTickTime;
	aiTick();
	for (int i = 0; i < bikesInGame(); i++)
		getBike(i)->onPhysicsTick();
	collideAllBikes();
}

