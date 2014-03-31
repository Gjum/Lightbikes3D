#include "Game.h"

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "Settings.h"

Game::Game() {
	srand(time(NULL));
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
		bike->pos.x = (mapSizeX - (4.0*bikeRadius * bikesNum/2.0)) / 2.0 + 2.0*bikeRadius * (i/2)*2;
		bike->pos.z = (i%2) == 0 ? mapSizeZ - 5*bikeRadius : 5*bikeRadius;
		bike->direction = (2*i) % 4;
		bike->speed = defaultBikeSpeed;
		bike->wallHeight = 1;
		bike->resetWalls();
		if (i%2 == 0) bike->setColor(1, (i/2)/(bikesNum/2.0), 0);
		else bike->setColor(0, (i/2)/(bikesNum/2.0), 1);
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

bool Game::allBikesDead() {
	for (int i = 0; i < bikesInGame(); i++) {
		if (!getBike(i)->isDead())
			return false;
	}
	return true;
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
		if (!getBike(i)->isDead()) {
			bikesLeft++;
			if (bikesLeft >= 2) return false ;
			lastLivingBike = i;
		}
	}
	// assume bikesLeft < 2
	if (lastLivingBike == -1) printf("Tie!\n");
	else printf("Bike %i wins! (%s)\n", lastLivingBike, (lastLivingBike%2 == 0) ? "Red" : "Blue");
	return true;
}

bool Game::canGoForward(Bike *bike) {
	if (bike->isDying()) return false;
	Bike *ghost = new Bike(bike);
	bike->wallHeight = 0; // simulate death to be ignored on collision tests
	// move twice to avoid frontal crash with other bike
	ghost->onPhysicsTick();
	ghost->onPhysicsTick();
	bool ret = !(collideBikeWithEverything(ghost) || ghost->collideWithWalls(ghost));
	delete ghost;
	bike->wallHeight = 1;
	return ret;
}

bool Game::canTurn(Bike *bike, bool right) {
	if (bike->isDying()) return false;
	Bike *ghost = new Bike(bike);
	bike->wallHeight = 0; // simulate death to be ignored on collision tests
	// move twice to avoid frontal crash with other bike
	ghost->turn(right);
	ghost->onPhysicsTick();
	ghost->onPhysicsTick();
	bool ret = !(collideBikeWithEverything(ghost) || ghost->collideWithWalls(ghost));
	delete ghost;
	bike->wallHeight = 1;
	return ret;
}

bool Game::preferredTurnSide(Bike *bike) {
	if (bike->isDying()) return false;
	bool right = random()%2 == 0; // 1:1 left or right, if both do not collide
	Bike *bikeLeft = new Bike(bike);
	Bike *bikeRight = new Bike(bike);
	bikeLeft->turn(false);
	bikeRight->turn(true);
	// move away from each other
	bikeLeft->onPhysicsTick();
	bikeLeft->onPhysicsTick();
	bikeLeft->onPhysicsTick();
	bikeRight->onPhysicsTick();
	bikeRight->onPhysicsTick();
	bikeRight->onPhysicsTick();
	for (int i = 0; i < 50; i++) {
		bikeLeft->onPhysicsTick();
		bikeRight->onPhysicsTick();
		// left collides => turn right and vice versa
		if (collideBikeWithEverything(bikeLeft) || bikeLeft->collideWithWalls(bikeLeft)) {
			right = true;
			break;
		}
		else if (collideBikeWithEverything(bikeRight) || bikeRight->collideWithWalls(bikeRight)) {
			right = false;
			break;
		}
	}
	delete bikeLeft;
	delete bikeRight;
	return right;
}

void Game::aiTick() {
	static unsigned long ticksSinceTurn[bikesNum]; // uninitialized, so it's >0 but random
	static unsigned long ticksSinceForcedTurn[bikesNum];
	// start at 2, because 0 and 1 are the players
	for (int i = 2; i < bikesInGame(); i++) {
		ticksSinceTurn[i]++;
		ticksSinceForcedTurn[i]++;
		Bike *bike = getBike(i);
		if (bike->isDying()) continue;
		bool turn = false;
		bool right = preferredTurnSide(bike);
		if (!canGoForward(bike)) { // bike is about to crash, but could dodge
			ticksSinceForcedTurn[i] = 0;
			turn = true;
			// is it safe to move into this direction?
			if (!canTurn(bike, right)) {
				right = !right;
			}
		}
		else if (ticksSinceTurn[i] > 10
				&& ticksSinceForcedTurn[i] > 100
				&& random()%100 == 0) { // wander, i.e. randomly turn
			turn = true;
			if (!canTurn(bike, right)) {
				right = !right;
			}
		}
		bike->wallHeight = 1;
		if (turn) {
			bike->turn(right);
			ticksSinceTurn[i] = 0;
		}
	}
}

void Game::physicsTick() {
	secondsToNextPhysicsTick += physicsTickTime;
	aiTick();
	for (int i = 0; i < bikesInGame(); i++)
		getBike(i)->onPhysicsTick();
	collideAllBikes();
}

