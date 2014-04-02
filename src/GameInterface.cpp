#include "GameInterface.h"

GameInterface::GameInterface() {
}

GameInterface::~GameInterface() {
}

void GameInterface::newGame() {
}

bool GameInterface::onFrame(float frameSec) {
}

void GameInterface::turnBike(int controllerID, bool right) {
}

int GameInterface::addController(Controller *controller) {
}

void GameInterface::removeController(int controllerID) {
}

void GameInterface::closeGame() {
}

int GameInterface::controllersInGame() {
}

Bike *GameInterface::getBike(int controllerID) {
}

int GameInterface::nextLivingController(int start, bool next) {
}

bool GameInterface::allControllersDead() {
}

bool GameInterface::collideBikeWithEverything(Bike *bike) {
}

void GameInterface::collideAllBikes() {
}

void GameInterface::killBike(int controllerID) {
}

bool GameInterface::testForGameOver() {
}

void GameInterface::physicsTick() {
}

