#include "Game.h"

#include <cstdio>
#include "Settings.h"

Game::Game() {
	bikes.clear();
	gameClosed = false;
}

Game::~Game() {
	for (int i = 0; i < bikesInGame(); i++)
		if (getBike(i)->controller != 0)
			delete getBike(i)->controller;
}

void Game::newGame() {
	printf("Game restart\n");
	for (int i = 0; i < bikesInGame(); i++) {
		Bike *bike = getBike(i);
		bike->pos.x = (mapSizeX - (4.0*bikeRadius * bikesInGame()/2.0)) / 2.0 + 2.0*bikeRadius * (i/2)*2;
		bike->pos.z = (i%2) == 0 ? mapSizeZ - 5*bikeRadius : 5*bikeRadius;
		bike->direction = (2*i) % 4;
		bike->speed = defaultBikeSpeed;
		bike->wallHeight = 1;
		bike->resetWalls();
		if (i%2 == 0) bike->setColor(1, (i/2)/(bikesInGame()/2.0), 0);
		else bike->setColor(0, (i/2)/(bikesInGame()/2.0), 1);
	}
	for (int i = 0; i < bikesInGame(); i++)
		if (getBike(i)->controller != 0)
			getBike(i)->controller->onNewGame();
	secondsToNextPhysicsTick = 0;
}

bool Game::onFrame(float frameSec) {
	secondsToNextPhysicsTick -= frameSec;
	while (secondsToNextPhysicsTick < 0) {
		for (int i = 0; i < bikesInGame(); i++)
			if (getBike(i)->controller != 0)
				getBike(i)->controller->updateControls();
		physicsTick();
	}
	for (int i = 0; i < bikesInGame(); i++)
		if (getBike(i)->controller != 0)
			getBike(i)->controller->updateView(frameSec);
	if (testForGameOver()) newGame();
	return gameClosed;
}

void Game::addController(Controller *controller) {
	Bike *bike = new Bike(controller);
	bikes.push_back(bike);
}

void Game::closeGame() {
	gameClosed = true;
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

void Game::physicsTick() {
	secondsToNextPhysicsTick += physicsTickTime;
	for (int i = 0; i < bikesInGame(); i++)
		getBike(i)->onPhysicsTick();
	collideAllBikes();
}

