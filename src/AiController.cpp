#include "AiController.h"

#include <cstdlib>
#include <ctime>

AiController::AiController(Game *game, int bikeID) {
	srand(time(NULL));
	this->game = game;
	bike = game->getBike(bikeID);
}

AiController::~AiController() {
}

void AiController::onNewGame() {
}

void AiController::updateControls() {
	if (bike->isDying()) return;
	static unsigned long ticksSinceTurn = 0;
	static unsigned long ticksSinceForcedTurn = 0;
	ticksSinceTurn++;
	ticksSinceForcedTurn++;
	bool turn = false;
	bool right = preferredTurnSide();
	if (!canGoForward()) { // bike is about to crash, but could dodge
		ticksSinceForcedTurn = 0;
		turn = true;
		// is it safe to move into this direction?
		if (!canTurn(right)) {
			right = !right;
		}
	}
	else if (ticksSinceTurn > 10
			&& ticksSinceForcedTurn > 100
			&& random()%100 == 0) { // wander, i.e. randomly turn
		turn = true;
		if (!canTurn(right)) {
			right = !right;
		}
	}
	bike->wallHeight = 1;
	if (turn) {
		bike->turn(right);
		ticksSinceTurn = 0;
	}
}

void AiController::updateView(float frameSec) {
	// do nothing
}

bool AiController::canGoForward() {
	if (bike->isDying()) return false;
	Bike *ghost = new Bike(bike);
	bike->wallHeight = 0; // simulate death to be ignored on collision tests
	// move twice to avoid frontal crash with other bike
	ghost->onPhysicsTick();
	ghost->onPhysicsTick();
	bool ret = !(game->collideBikeWithEverything(ghost) || ghost->collideWithWalls(ghost));
	delete ghost;
	bike->wallHeight = 1;
	return ret;
}

bool AiController::canTurn(bool right) {
	if (bike->isDying()) return false;
	Bike *ghost = new Bike(bike);
	bike->wallHeight = 0; // simulate death to be ignored on collision tests
	// move twice to avoid frontal crash with other bike
	ghost->turn(right);
	ghost->onPhysicsTick();
	ghost->onPhysicsTick();
	bool ret = !(game->collideBikeWithEverything(ghost) || ghost->collideWithWalls(ghost));
	delete ghost;
	bike->wallHeight = 1;
	return ret;
}

bool AiController::preferredTurnSide() {
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
		if (game->collideBikeWithEverything(bikeLeft) || bikeLeft->collideWithWalls(bikeLeft)) {
			right = true;
			break;
		}
		else if (game->collideBikeWithEverything(bikeRight) || bikeRight->collideWithWalls(bikeRight)) {
			right = false;
			break;
		}
	}
	delete bikeLeft;
	delete bikeRight;
	return right;
}

