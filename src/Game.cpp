#include "Game.h"

#include <cstdio>
#include "Settings.h"

Game::Game() {
	bikes.clear();
	for (int i = 0; i < bikesNum; i++) {
		Bike *bike = new Bike();
		bikes.push_back(bike);
	}
}

Game::~Game() {
}

void Game::newGame() {
	printf("Game restart\n");
	for (int i = 0; i < bikesNum; i++) {
		Bike *bike = bikes.at(i);
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
	for (int i = 0; i < bikes.size(); i++)
		bikes.at(i)->move(frameSec);
	collideAllBikes();
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
		if (tries++ > bikes.size()) {
			printf("ERROR nextLivingBike got into an infinite loop\n");
			return start;
		}
		newBikeID += next ? 1 : bikes.size()-1;
		newBikeID %= bikes.size();
	} while (bikes.at(newBikeID)->isDead());
	return newBikeID;
}

bool Game::collideBikeWithEverything(Bike *bike) {
	if (bike->collideWithMapBorder())
		return true;
	for (int i = 0; i < bikes.size(); i++) {
		Bike *otherBike = bikes.at(i);
		if (otherBike->isDead()) continue;
		if (bike->collideWithWalls(otherBike))
			return true;
		if (otherBike != bike && bike->collideWithBike(otherBike))
			return true;
	}
	return false;
}

void Game::collideAllBikes() {
	for (int i = 0; i < bikes.size(); i++)
		if (collideBikeWithEverything(bikes.at(i)))
			killBike(i);
}

void Game::killBike(int bikeID) {
	Bike *bike = bikes.at(bikeID);
	if (bike->isDying()) return;
	bike->wallHeight = 0.9999;
	printf("Bike %i crashed\n", bikeID);
}

bool Game::testForGameOver() {
	int bikesLeft = 0;
	int lastLivingBike = -1;
	for (int i = 0; i < bikes.size(); i++) {
		if (!bikes.at(i)->isDying()) {
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

